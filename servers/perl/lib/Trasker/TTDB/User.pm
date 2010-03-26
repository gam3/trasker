use strict;
## @file
# The User object
#

## @class
# The User Object.
#
# A user is normally a human, but can be any object that can be in only
# one place at one time.
#
package Trasker::TTDB::User;

use Trasker::TTDB::DBI qw (get_dbh);

use Trasker::TTDB::Projects;

our $VERSION = '0.001';

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT ARRAYREF );

use Carp qw (croak);

=head1 NAME

Trasker::TTDB::User - Perl interface to the tasker user

=head1 SYNOPSIS

  use Trasker::TTDB::User;

  $user = Trasker::TTDB::User->new(user => 'bob', fullname => 'Robert Smith'):

  $user->create();

  $user = Trasker::TTDB::User->get(user => 'bob'):
  $user = Trasker::TTDB::User->get(id => 1):

=head2 Constructor

=over

=item new(name => I<required>, fullname => I<required>)

This creates a user object.  Use I<create> to make this object
preminate.

=back

=cut

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        id => 0,
        name => 1,
        fullname => 1,
    });

    return bless { %p }, $class;
}

=head2 Methods

=over

=cut

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

=item get( { id => # | name => 'name' } )

I<get> returns a user object that describes the user requested
by either I<name> or I<id>.

=cut

sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
        id => 0,
        user => 0,
    });

    croak(q(Can't have both a user and an id)) if (defined $p{user} && defined $p{id});
    croak(q(Must have a user or an id)) if (!defined $p{user} && !defined $p{id});

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

    die "No User $p{id}" unless $data;

    return bless { %$data }, $class;
}

sub id
{
    my $self = shift;

    $self->{id} || die "No id";
}

sub times
{
    my $self = shift;
    my %p = validate(@_, {
        date => {
	    isa => 'Trasker::Date',
	},
    });
    my $dbh = get_dbh();
    my $date_clause;
    my @args = ();

    if ($p{date}) {
	$date_clause = <<EOP;
and (start_time < ? and (end_time >= ? or end_time is NULL))
EOP
        push(@args, ($p{date} + 1)->mysql);
        push(@args, ($p{date} + 0)->mysql);
    }
    my $sth = $dbh->prepare(<<SQL);
select timeslice.id,
       users.name as user_name,
       project.id as project_id,
       project.name as project_name,
       start_time,
       end_time,
       auto_id,
       host,
       'eof'
  from timeslice, project, users
 where users.id = timeslice.user_id
   and project.id = project_id
   and users.id = ?
   $date_clause
 order by start_time
 limit 100
SQL
#   warn $sth->{Statement};
    $sth->execute($self->id, @args);

    my @data;

    require Trasker::TTDB::TimeSlice;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'Trasker::TTDB::TimeSlice');
    }

    @data;
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

    require Trasker::TTDB::UserProject;

    my $prjs = Trasker::TTDB::Projects->new(user => $self);
    my @ret;

    for my $project ($prjs->entries) {
        my $up = Trasker::TTDB::UserProject->new(user => $self, project => $project);
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
        name => 0,
    });

    my $id = $p{id};

    my $project;
    if (my $p = $p{project}) {
        $project = $p;
    } elsif (my $name = $p{name}) {
        $project = Trasker::TTDB::Project->get(name => $name);
    } elsif (my $id = $p{id}) {
        $project = Trasker::TTDB::Project->get(id => $id);
    }

    require Trasker::TTDB::UserProject;

    Trasker::TTDB::UserProject->new(project => $project, user => $self);
}

=item set_current_project

=cut

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
           isa => [ qw(  Trasker::TTDB::Project ) ],
        },
    });

    $p{host} ||= '_unknown_';

    $p{temporary} ||= 'normal';

    $p{auto_id} ||= 0;

    my $project_id = defined $p{project} ? $p{project}->id() : $p{project_id};

    my $dbh = get_dbh('commit');

    my $sths = $dbh->prepare(<<SQL) or die chomp $dbh->err_str();
select id, project_id, temporary, revert_to
  from timeslice
 where end_time is NULL
   and user_id = ?
   for update
SQL

    my $sthu;
    if (0) {
        $sthu = $dbh->prepare(<<'SQL') or die $dbh->err_str();
update timeslice
   set elapsed = timediff(NOW(), start_time),
   end_time = NOW(),
 where id = ?
SQL
    } else {
        $sthu = $dbh->prepare(<<'SQL') or die $dbh->err_str();
update timeslice
   set elapsed = now() - start_time,
   end_time = now()
 where id = ?
SQL
    }

    my $sthi = $dbh->prepare(<<SQL) or die $dbh->err_str();
insert into timeslice
       (user_id, project_id, temporary, start_time, auto_id, revert_to, host)
values (      ?,          ?,         ?,      now(),       ?,         ?,    ?)
SQL

    eval {
        $sths->execute($user_id);       # get current project

        my ($id, $current_project_id, $type, $old_rid) = (undef, 0, undef, 0);

        my $rows = $sths->rows;

        if ($rows == 1) {
            ($id, $current_project_id, $type, $old_rid) = ($sths->fetchrow_array);

            if ($project_id == $current_project_id) {
warn("Not updating $current_project_id == $project_id");
                $dbh->rollback;
                return 0;
            }
            $sthu->execute($id);   # end current timeslice
            die 'No update' unless $sthu->rows == 1;
        } elsif ($rows == 0) {
#           warn "new";
        } else {
            die "The database is corrupt";
        }

        my $new_rid;
        if ($p{temporary} ne 'normal') {
           $new_rid = $old_rid || $id;
        }

        $sthi->execute(
          $user_id,
          $project_id,
          $p{temporary},
          $p{auto_id} || undef,
          $new_rid,
          $p{host},
        ) || die $sthi->{Statement} . ' ' . $dbh->errstr ;
    };
    if ($@) {
        $dbh->rollback;
	die $@;
    } else {
        $dbh->commit;
    }

    return 1;
}

sub revert
{
    my $self = shift;
    my $user_id = $self->id();

    my %p = validate(@_, {
        type => 0,
        host => 0,
    });

    $p{type} ||= 'xnormal';

    my $dbh = get_dbh('commit');

    my $sth = $dbh->prepare(<<SQL);
select id,
       temporary,
       revert_to,
       project_id,
       host
  from timeslice
 where revert_to is not NULL
   and elapsed is NULL
   and user_id = ? for update;
SQL

    my $stha = $dbh->prepare(<<SQL) or die;
update timeslice
   set elapsed = now() - start_time, end_time = now()
 where id = ?
SQL
    my $sthi = $dbh->prepare(<<SQL) or die $dbh->err_str();
insert into timeslice (user_id, project_id, temporary, auto_id, start_time, host)
               SELECT user_id, project_id, 'revert', auto_id, now(), host FROM timeslice where id = ?
SQL

    $sth->execute($user_id);

    my $rows = $sth->rows;

    if ($rows == 1) {
        my ($id, $flag, $rid, $pid, $host) = $sth->fetchrow_array;

        if ($flag eq $p{type} and defined $rid and $rid > 0) {
            eval {
                $stha->execute($id);
                $sthi->execute($rid);
            };
            if ($@) {
                $dbh->rollback;
            } else {
                $dbh->commit;
            }
        } else {
            warn "Rollback";
        }
    } elsif ($rows == 0) {
        warn "Nothing to revert too." if ($Trasker::TTDB::debug);
    } else {
        die "Fatal Error: ", $rows;
    }
}

sub auto_revert_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        id => 1,
    });

    $self->revert(type => 'window', id => $p{host});  # We only want to revert it if we set it.
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

    $pid;
}

=item auto_set_project

=cut

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

    require Trasker::TTDB::Auto;

    my $auto = Trasker::TTDB::Auto->get(
        user => $self,
        role => '',
        %p,
    );

    if (defined $auto) {
#        my $newproject = $self->project($auto->project_id);
#        my $curproject = $self->current_project();

        return $self->set_current_project(project_id => $auto->project_id, temporary => 'window', auto_id => $auto->id, host => $p{host});
    } else {
        $self->revert(type => 'window', host => $p{host});
        return -1;
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

    require Trasker::TTDB::Project;
    require Trasker::TTDB::UserProject;

    Trasker::TTDB::UserProject->new(project => Trasker::TTDB::Project->get(id => $current), user => $self);
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
           isa => [ qw(  Trasker::TTDB::Project ) ],
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

    my $dbh = get_dbh('commit');

    my $uid = $self->id() || die;
    my %p = validate(@_, {
        name => 1,
        description => 1,
    });

    my $st_id = $dbh->prepare(qq/select nextval('project_id_seq')/);
    my $st = $dbh->prepare("insert into project (id, user_id, parent_id, name, description) values (?, ?, NULL, ?, ?)");

    my $id;
    eval {
        $st_id->execute();
        $id = $st_id->fetchrow();

        $st->execute($id, $self->id(), $p{name}, $p{description}) or die $dbh->errstr();
    };
    if ($@) {
        $dbh->rollback;
        die $@;
    } else {
        $dbh->commit;
    }

    Trasker::TTDB::Projects::flush();

    Trasker::TTDB::UserProject->new(user => $self, project_id => $id);
}

=item get_alerts

  $user->get_alerts();

will die

=cut

sub get_alerts
{
    require Trasker::TTDB::Alerts;
    die "not impliemnted";
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
    my $start = Trasker::Date->new($day->date, 0,0,0);

    my $dbh = get_dbh();


    my $project_id_clause = '';
    my @args;
    if ($a = $p{pids}) {
        $project_id_clause = sprintf("and project_id in (%s)", join(', ', map({'?'} @$a)));
        push(@args, @$a);

    }

    my $sth = $dbh->prepare(<<SQL);
select
       sum(
           coalesce(
           case when date(end_time) >= date(?) + interval '1 day' then date(?) + interval '1 day' else end_time end,
           now())  -
           case when date(start_time) < date(?) then date(?) else start_time end) as time
  from timeslice
  where start_time < date(?) + interval '1 day'
    and coalesce(end_time, now()) >= date(?)
    and user_id = ?
    $project_id_clause
SQL

    $sth->execute(
        $start->mysql,
        $start->mysql,
        $start->mysql,
        $start->mysql,
        $start->mysql,
        $start->mysql,
        $self->id,
        @args,
    );

    my @data;

    my $data = $sth->fetchrow_hashref() || { time => '0:00:00.0' };

    return bless({ %p, data => $data, date => $start }, 'Trasker::TTDB::Time');
}

sub days
{
    my $self = shift;
    my %p = validate(@_, {
        start => { isa => 'Date::Calc' },
        end => { isa => 'Date::Calc' },
        pids => 0,
    });

    die 'need a date' if ($p{start}->is_long);
    die 'need a date' if ($p{end}->is_long);

    my $day = $p{start};
    my $start = Trasker::Date->new($day->date, 0,0,0);
    $day = $p{end};
    my $end = Trasker::Date->new($day->date, 0,0,0);

    my $dbh = get_dbh();

    my $project_id_clause;
    my @args;
    if ($a = $p{pids}) {
        $project_id_clause = sprintf("and project_id in (%s)", join(', ', map({'?'} @$a)));
        push(@args, @$a);

    }

    my $sth = $dbh->prepare(<<SQL);
select
       sum(
           coalesce(
           case when date(end_time) >= date(?) + interval '1 day' then date(?) + interval '1 day' else end_time end,
           now())  -
           case when date(start_time) < date(?) then date(?) else start_time end) as time
  from timeslice
  where start_time < date(?) + interval '1 day'
    and coalesce(end_time, now()) >= date(?)
    and user_id = ?
    $project_id_clause
SQL

    $sth->execute(
        $end->mysql,
        $end->mysql,
        $start->mysql,
        $start->mysql,
        $end->mysql,
        $start->mysql,
        $self->id,
        @args,
    );

    my @data;

    my $data = $sth->fetchrow_hashref() || { time => '0:00:00.0' };

    return bless({ %p, data => $data, date => $start }, 'Trasker::TTDB::Time');
}


=item recent_projects

  return the last (10) or fewer projects that have been used in the last 10 days.

=cut

sub recent_projects
{
    my $self = shift;
    my $dbh = get_dbh('read');

    my $sth = $dbh->prepare(<<SQL);
select project_id,
       sum(elapsed) as time,
       count(*)
  from timeslice
 where user_id = ?
   and start_time + interval '10 days' > now()
 group by project_id
 having count(*) > 1
 order by time desc
 limit 10
SQL

    $sth->execute($self->id);

    my $data = $sth->fetchall_arrayref({});

    $sth->finish;

    my $ret = bless({ user_id => $self->id }, "Trasker::TTDB::ProjectResponse");

    for my $entry (@$data) {
	$data = { %$entry };
	$ret->{project}{$entry->{project_id}} = $data;
	push(@{$ret->{projects}},  $entry->{project_id});
    }

    return $ret;
}

=item get_timeslices_for_day

=cut

sub get_timeslices_for_day
{
    my $self = shift;
    my %p = validate(@_, {
        date => {
	    isa => 'Trasker::Date',
	},
    });
    my $dbh = get_dbh();
    my $date_clause;
    my @args = ();

    if ($p{date}) {
	$date_clause = <<EOP;
and (start_time < ? and (end_time >= ? or end_time is NULL))
EOP
        push(@args, ($p{date} + 1)->mysql);
        push(@args, ($p{date} + 0)->mysql);
    }
    my $sth = $dbh->prepare(<<SQL);
select *
  from timeslice
 where user_id = ?
       $date_clause
 order by start_time
 limit 10000
SQL
    $sth->execute($self->id, @args);

    my @data;

    require Trasker::TTDB::TimeSlice;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, Trasker::TTDB::TimeSlice->new(%$data));
    }

    @data;
}

=item timesplit

  $user->timesplit(
      timeslice_id => "10",
      time => "11:00:00",
  );

returns a list of the timeslice ids for the timeslices created or updated.

=cut

sub timesplit
{
    my $self = shift;
    my $dbh = get_dbh('commit');
    my %p = validate(@_, {
	timeslice_id   => 1,
	time => 0,
	times => {
	    optional => 1,
	    type => ARRAYREF,
	},
    });
    my $curid = $p{timeslice_id};
    my @times = ();
    push(@times, $p{time}) if defined $p{time};
    push(@times, @{$p{times}||[]});

    my $sth_id = $dbh->prepare(qq/select currval('timeslice_id_seq')/);

    my $sth_s = $dbh->prepare(<<SQL);
select *
  from timeslice
 where id = ?
   and user_id = ?
   for update;
SQL

    my $sthi = $dbh->prepare(<<SQL);
insert into timeslice
       (user_id, project_id, temporary, start_time, end_time, auto_id, revert_to, host, elapsed)
   select user_id, project_id, temporary, end_time, end_time, auto_id, revert_to, host, end_time - end_time
     from timeslice where id = ?
SQL

    my $sth_st = $dbh->prepare(<<SQL);
update timeslice
   set start_time = ?,
       elapsed = end_time - ?
 where id = ?
SQL

    my $sth_et = $dbh->prepare(<<SQL);
update timeslice
   set end_time = ?,
       temporary = 'split',
       elapsed = ? - start_time
 where id = ?
SQL
    my @timeslices = ($curid);
    eval {
	$sth_s->execute($curid, 1);		# Lock the timeslice being split

	die "Timeslice not found" unless $sth_s->rows;

	my $data = $sth_s->fetchrow_hashref();

	require Trasker::TTDB::TimeSlice;

	my $orig = Trasker::TTDB::TimeSlice->new(%$data);

	my @splittimes = sort({ die "duplicate time $a $b" if $a eq $b; $b cmp $a } map({ref($_) ? $_ : $orig->end_time->replace_time($_); } @times));

	foreach my $splittime (@splittimes) {
	    warn $splittime;

	    if ($splittime ge $orig->end_time or $splittime le $orig->start_time) {
		die "Time out of range " . $splittime->mysql();
	    }

	    $sthi->execute($orig->id);

	    $sth_id->execute();

	    my $id = $sth_id->fetchrow();

	    push @timeslices, $id;

	    my $t = $splittime->mysql;
	    $sth_et->execute($t, $t, $curid);
	    $sth_st->execute($t, $t, $id);
	}
	$dbh->commit;
    };
    if ($@) {
	$dbh->rollback;
	die $@;
    }

    return @timeslices;
}



1;
__END__

=item  id

=item  userid

=item  name

This is an alias to the userid function

=item fullname

=item projects

=item project

=item revert

=item auto_revert_project

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

=item days

Return information about a give range of days for the user.

=item times

Get the timeslices for a given period

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
