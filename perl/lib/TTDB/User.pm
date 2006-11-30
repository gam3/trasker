use strict;

package TTDB::User;

use TTDB::DBI qw (get_dbh);

use TTDB::Projects;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Carp qw (croak);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
	name => 1,
	fullname => 1,
    });

    return bless { %p }, $class;
}

sub create
{
    my $self = shift;

    die if $self->{id};

    my $dbh = get_dbh;

    my $sth_id;
    if (0) {
	$sth_id = $dbh->prepare('select LAST_INSERT_ID()');
    } else {
	$sth_id = $dbh->prepare("select currval('users_id_seq')");
    }
    my $sth = $dbh->prepare(<<SQL);
insert into users (name, fullname) values (?, ?)
SQL

    $sth->execute($self->{name}, $self->{fullname});
    $sth_id->execute();
    my $id = $sth_id->fetchrow_array();
    $self->{id} = $id;

    $dbh->commit();

    $self;
}

sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
	id => 0,
	user => 0,
    });

    croak(q(Can't have both user and id)) if (defined $p{user} && defined $p{id});
    croak(q(Must have an user or id)) if (!defined $p{user} && !defined $p{id});

    my $sth;

    if (my $user = $p{user}) {
	$sth = $dbh->prepare(<<SQL);
select name,
       fullname,
       id,
       'eof' as eof
  from users
 where name = ?
SQL

	$sth->execute($user);
    } else {
	my $id = $p{id};
	$sth = $dbh->prepare(<<SQL);
select name,
       fullname,
       id,
       'eof'
  from users
 where id = ?
SQL

	$sth->execute($id);
    }

    my $data = $sth->fetchrow_hashref();

    die "No User" unless $data;

    return bless { %$data }, $class;
}

sub id
{
    my $self = shift;

    $self->{id} || die "No id";
}

sub userid
{
    my $self = shift;

    $self->{name};
}

*name = \&userid;

sub fullname
{
    my $self = shift;
    $self->{fullname};
}

sub projects
{
    my $self = shift;

    require TTDB::UserProject;

    my $up = TTDB::Projects->new(user => $self);
    my @ret;

    for my $project ($up->entries) {
	my $up = TTDB::UserProject->new(user => $self, project => $project);
	if ($up->active()) {
	    push(@ret, $up)
	}
    }

    @ret;
}

sub project
{
    my $self = shift;

    my %p = validate(@_, {
	id => 0,
	project => 0,
    });

    my $id = $p{id};

    my $project = $p{project} || TTDB::Project->get(id => $id);

require TTDB::UserProject;

    TTDB::UserProject->new(project => $project, user => $self);
}

sub set_current_project
{
    my $self = shift;
    my $user_id = $self->id;
    my $current;

    my %p = validate(@_, {
	temporary => 0,
	project_id => 0,
	auto_id => 0,
	host => 1,
	project => {
	   optional => 1,
	   isa => [ qw(  TTDB::Project ) ],
	},
    });

    $p{host} = 'bob';

    $p{temporary} ||= 'normal';

    $p{auto_id} ||= 0;

    my $id = defined $p{project} ? $p{project}->id() : $p{project_id};

    my $dbh = get_dbh('commit');

    my $sth = $dbh->prepare(<<SQL);
select project_id from timeslice where elapsed is NULL and user_id = ? for update;
SQL
    my $stha = $dbh->prepare(<<'SQL');
update timeslice 
   set elapsed = timediff(NOW(), start_time),
   end_time = NOW(),
   id = (@temp := id),
   temporary = (@btype := temporary),
   revert_to = (@rid := revert_to)
 where elapsed is null and user_id = ?
SQL
    my $sthc = $dbh->prepare('select @temp, @btype, @rid');
    my $sthb = $dbh->prepare(<<SQL);
insert into timeslice (user_id, project_id, temporary, start_time, auto_id, revert_to, host) values (?, ?, ?, NOW(), ?, ?, ?);
SQL

    $sth->execute($user_id);
    if ($sth->rows) {
        my ($project_id) = ($sth->fetchrow_array);
	if ($project_id == $id) {
warn("Not updating $id == $project_id");
	    $dbh->rollback;
	    return $self;
	}
    }
    $stha->execute($user_id);
    my ($rid, $type, $old_rid) = (undef, undef, undef);

    $type = '';
    $rid = '';

    if ($p{auto_id}) {
        $sthc->execute();
        ($rid, $type, $old_rid) = ($sthc->fetchrow_array);
    }

    $sthb->execute(
      $user_id,
      $id,
      $p{temporary},
      $p{auto_id},
      ($type eq $p{temporary} && defined $old_rid) ? $old_rid : $rid,
      $p{host},
    ) || die $dbh->errstr;

    $dbh->commit;

    $self;
}

sub revert
{
    my $self = shift;
    my $user_id = $self->id();

    my %p = validate(@_, {
        type => 0,
        host => 0,
    });

    $p{type} ||= 'normal';

    my $dbh = get_dbh('commit');
    my $sth = $dbh->prepare(<<SQL);
select temporary, revert_to, project_id, host from timeslice where revert_to is not NULL and elapsed is NULL and user_id = ? for update;
SQL
    my $stha = $dbh->prepare(<<SQL) or die;
update timeslice
   set elapsed = timediff(NOW(), start_time), end_time = NOW()
 where elapsed is null and user_id = ?;
SQL
    my $sthb = $dbh->prepare(<<SQL) or die;
insert into timeslice (user_id, project_id, temporary, auto_id, revert_to, start_time, host)
select user_id, project_id, temporary, auto_id, revert_to, now(), host from timeslice
 where id = ?
SQL

    $sth->execute($user_id);

    if ($sth->rows) {
	my ($flag, $rid, $pid, $host) = $sth->fetchrow_array;

	if ($flag eq $p{type} and defined $rid) {
	    eval {
	      $stha->execute($user_id) or die $dbh->err_str;
	      $sthb->execute($rid) or die $dbh->err_str;
	    }; if ($@) {
		$dbh->rollback;
	    } else {
		$dbh->commit;
	    }
	} else {
	    $dbh->rollback;
	}
    }
}

sub auto_revert_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
	host => 1,
	class => 1,
	title => 1,
	role => 0,
	name => 1,
	desktop => 1,
    });
    
    $self->revert(type => 'window', host => $p{host});  # We only want to revert it if we set it.
}

sub auto_get_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
	host => 1,
	class => 1,
	title => 1,
	name => 1,
	desktop => 1,
	role => 1,
    });
use Data::Dumper;
print Dumper \%p;
    
    my $st = $dbh->prepare(<<SQL);
select project_id, id, presidence from auto
 where ? like host
   and user_id = ?
   and ? like desktop
   and ? like name
   and ? like class
   and ? like title
   and ? like role
 order by presidence desc
 limit 1
SQL

    $st->execute($p{host}, $self->id, $p{desktop}, $p{name}, $p{class}, $p{role}, $p{title});

    my ($pid, $id, $presidence ) = $st->fetchrow_array;
print "Got $pid, $id, $presidence\n";
    $pid;
}

sub auto_set_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
	host => 1,
	class => 1,
	title => 1,
	name => 1,
	role => 0,
	desktop => 1,
    });
    
    my $st = $dbh->prepare(<<SQL);
select id, project_id, presidence from auto
 where ? like host
   and user_id = ?
   and ? like desktop
   and ? like class
   and ? like title
   and ? like name
 order by presidence desc
 limit 1
SQL

    $st->execute($p{host}, $self->id, $p{desktop}, $p{class}, $p{title}, $p{name});
    $p{user} = $self->id;

    my ($id, $project_id, $p) = $st->fetchrow_array;

    if ($st->rows) {
	$self->set_current_project(project_id => $project_id, temporary => 'window', auto_id => $id, host => $p{host});
    } else {
	$self->revert(type => 'window');
    }
}

sub current_project
{
    my $self = shift;
    my $user_id = $self->id();

    my $dbh = get_dbh;
    my $sth = $dbh->prepare(<<SQL);
select project_id from timeslice where elapsed is NULL and user_id = ?;
SQL

    $sth->execute($user_id);

    my $current = $sth->fetchrow_array;

    croak("No current project") unless $current;

    require TTDB::Project;
    require TTDB::UserProject;

    TTDB::UserProject->new(project => TTDB::Project->get(id => $current), user => $self);
}

sub has_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my $id = $self->id() || die;
    my %p = validate(@_, {
	id => 0,
	project => {
	   optional => 1,
	   isa => [ qw(  TTDB::Project ) ],
	},
    });
    die 'need a project' unless $p{id} || $p{project};
    die 'only one project' if $p{id} && $p{project};
    my $project_id = $p{id} || $p{project}->id;

    my $sth = $dbh->prepare(<<SQL);
select count(*)
  from user_project
 where user_id = ? and project_id = ?
SQL

    $sth->execute($id, $project_id);

    my $ret = $sth->fetchrow();

    $ret;
}

sub add_task
{
    my $self = shift;
    my $dbh = get_dbh;
    my $id = $self->id() || die;
    my %p = validate(@_, {
	project => {
	   isa => [ qw(  TTDB::Project ) ],
	},
    });
}

sub add_note
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        note => 1,
        time => {
	    optional => 1,
	    isa => "Date::Calc",
	},
    });

    my $st;

    if ($p{time}) {
	$st = $dbh->prepare("insert into notes (type, time, user_id, note) values (2, ?, ?, ?)");

	$st->execute($p{time}->mysql, $self->id(), $p{note}) or die $dbh->err_str();
    } else {
	$st = $dbh->prepare("insert into notes (type, time, user_id, note) values (2, now(), ?, ?)");

	$st->execute($self->id(), $p{note}) or die $dbh->err_str();
    }

    $dbh->commit or die $dbh->err_str();

    $self;
}

sub delete
{
    my $self = shift;
    my $dbh = get_dbh;

    my $st = $dbh->prepare("delete from user where id = ?");

    $st->execute($self->{id});

    $self;
}

sub day
{
    my $self = shift;
    my %p = validate(@_, {
        date => { isa => 'Date::Calc' },
        pids => 0,
    });

    die 'need a date' if ($p{date}->is_long);

    my $day = $p{date};
    my $start = Date::Calc::MySQL->new($day->date, 0,0,0);
    my $end = Date::Calc::MySQL->new(($day+1)->date, 0,0,0);

    my $dbh = get_dbh();

    my $project_id_clause = "and project_id in (1, 2, 3, 4)";

    my $sth = $dbh->prepare(<<SQL);
select id, 
       project_id,
       timeslice.user_id,
       start_time,
       end_time,
       sec_to_time(
        sum(
	 time_to_sec(
	  timediff(
	   if(date(end_time) = cast(? as date), end_time, cast(? as datetime)),
           if(date(start_time) = cast(? as date), start_time, cast(? as datetime))
	  )
	 )
        )
       ) 'time',
       'eof'
  from timeslice
  where date(start_time) <= cast(? as date)
    and date(end_time) >= cast(? as date)
    and user_id = ?
    $project_id_clause
    group by user_id, project_id
SQL
    $sth->execute(
	$day->mysql, $end->mysql, $day->mysql, $start->mysql,
	$day->mysql, $day->mysql,
	$self->id
    );

    my @data;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, date => $start }, 'TTDB::Time');
    }

    @data;
}


1;
__END__

=head1 NAME

TTDB::User - Perl interface to the tasker user

=head1 SYNOPSIS

  use TTDB::User;

  $user = TTDB::User->new(user => 'bob', fullname => 'Robert Smith'):

  $user->create();

  $user = TTDB::User->get(user => 'bob'):
  $user = TTDB::User->get(id => 1):

=head2 Constructor

=over

=item new

=item get

=back

=head2 Methods

=over

=item  id

=item  userid

=item  name

This is an alias to the userid function

=item  fullname

=item  projects

=item  project

=item set_current_project
=item revert
=item auto_revert_project
=item auto_set_project

=item auto_get_project

=item current_project
=item has_project

=item add_task
=item add_note

=item create

This will create a user.  The name must be unique.

=item delete

This will delete a user, but only if the user has never been active.

=item day

Return information about a give day for the user.

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

