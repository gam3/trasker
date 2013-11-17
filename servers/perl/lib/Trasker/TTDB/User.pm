use strict;
## @file
# The User object
#

use v5.10.0;

## @class
# The User Object.
#
# A user is normally a human, but can be any object that can be in only
# one place at one time.
#
package Trasker::TTDB::User;

use Trasker::TTDB::DBI qw (get_dbh dbtype get_sti);

our $VERSION = '0.002_001';

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT ARRAYREF );

use Carp qw (croak);

=head1 NAME

Trasker::TTDB::User - Perl interface to the tasker user

=head1 SYNOPSIS

  use Trasker::TTDB::User;

  $user = Trasker::TTDB::User->new(user => 'bob', fullname => 'Robert Smith'):
  $user->create();

  $user = Trasker::TTDB::User->create(user => 'bob', fullname => 'Robert Smith'):

  $user = Trasker::TTDB::User->get(user => 'bob'):
  $user = Trasker::TTDB::User->get(id => 1):

=head2 Constructor

=over

=item new(name => I<required>, fullname => I<required>)

This creates a user object.  Use I<create> to make this object preminate.

In most cases the I<create> constructor should be used in place of this constructor.

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

    die sprintf("No User with %s: %s", $p{user} ? 'name' : 'id',  $p{user} || $p{id}) unless $data;

    return bless { %$data }, $class;
}

=item $user->create()

=item Trasker::TTDB::User->create(user => 'name', fullname => 'Full Name'):

I<create> creates an entry in the database from an User object or arguments.

=back

=cut

sub create
{
    my $self = shift;

    if (ref($self)) {
	die 'User already exists' if $self->{id};
    } else {
        $self = bless({ @_ }, $self);
    }

    my $dbh = get_dbh;
    my $sth = get_sti('user_create');

    $sth->execute($self->{name}, $self->{fullname});

    my $id = $dbh->last_insert_id("","","","");
    $self->{id} = $id;

    $sth = get_sti('timeslice_init');

    $sth->execute($id);

    $dbh->commit();

    $self;
}

=head2 Accessor Methods

=over

=cut

sub id
{
    my $self = shift;

    $self->{id} || die "No id";
}

=item times

=cut

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
and (ts.start_time < ? and (te.start_time >= ? or te.start_time is NULL))
EOP
        push(@args, ($p{date} + 1)->mysql);
        push(@args, ($p{date} + 0)->mysql);
    }
    my $sth = $dbh->prepare(<<SQL);
select ts.id,
       users.name as user_name,
       project.id as project_id,
       project.name as project_name,
       ts.start_time,
       te.start_time as end_time,
       ts.auto_id,
       ts.host,
       'eof'
  from timeslice ts
       join project on project.id = ts.project_id
       join users on users.id = ts.user_id
  left join timeslice te on te.id = ts.end_id
 where 
   users.id = ?
   $date_clause
 order by start_time
 limit 100
SQL
    $sth->execute($self->id, @args);

    my @data;

    require Trasker::TTDB::TimeSlice;

    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{id} }, 'Trasker::TTDB::TimeSlice');
    }
    $sth->finish();

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

=back 

=head2 Methods

=over

=cut

=item current_project

returns a C<UserProject> that represents the Current Project for a user.

=cut

sub current_project
{
    my $self = shift;
    my $user_id = $self->id();

    my $dbh = get_dbh;
    my $sth = $dbh->prepare(<<SQL);
select project_id from timeslice where end_id is NULL and user_id = ?;
SQL

    $sth->execute($user_id);

    my $current = $sth->fetchrow_array;

    $sth->finish();

    croak("No current project for $user_id") unless $current;

    require Trasker::TTDB::Project;
    require Trasker::TTDB::UserProject;

    Trasker::TTDB::UserProject->new(project => Trasker::TTDB::Project->get(id => $current), user => $self);
}

=item projects

=cut

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
           isa => [ qw( Trasker::TTDB::Project ) ],
        },
    });

    $p{host} //= '_unknown_';

    $p{temporary} //= 'normal';

    $p{auto_id} //= 0;

    my $project_id = defined $p{project} ? $p{project}->id() : $p{project_id};

    die "Need a project_id" unless defined $project_id;

    my $dbh = get_dbh('commit');

    my $sths = get_sti('timeslice_get_current');

    my $sthu = get_sti('timeslice_finish');

    my $sthi = get_sti('timeslice_new');

    eval {
        $sths->execute($user_id);       # get current project

        my ($id, $current_project_id, $type, $old_rid, $old_auto_id) = (undef, 0, undef, 0, undef);

        my $rows = $sths->rows;

        if ($rows == 1) {
            ($id, $current_project_id, $type, $old_rid, $old_auto_id) = ($sths->fetchrow_array);
	    $sths->finish;

            if ($project_id == $current_project_id &&
	        $p{auto_id} == $old_auto_id) {
warn("Not updating $current_project_id == $project_id");
                $dbh->rollback;
                return 0;
            }
        } elsif ($rows == 0) {
           warn "Timeslice was empty";
        } else {
            die "The database is corrupt";
        }

        my $new_rid;
        if ($p{temporary} ne 'normal') {
           $new_rid = $old_rid || $id;
        }

        $sthi->execute(
          $user_id,	           # user_id
          $project_id,
          $p{temporary},
          $p{auto_id} || undef,
          $new_rid,
          $p{host},
        );
	my $nid = $dbh->last_insert_id("","","timeslice","id");

	$sthu->execute($nid, $id);   # end current timeslice
	die 'No update: ' . $sthu->rows unless $sthu->rows == 1;
    };
    if ($@) {
warn "asdfasdf ", $@;
    }

    $sths->finish();
    $sthi->finish();
    $sthu->finish();

    if ($@) {
        my $error = $@;
        $dbh->rollback;
	die $error;
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
   and user_id = ?
SQL
# for update
    my $now = 'now()';
    if (dbtype eq 'sqlite') {
        $now = q[datetime('now')];
    }

    my $sthi = $dbh->prepare(<<SQL);
insert into timeslice (user_id, project_id, temporary, auto_id, start_time, host)
                SELECT user_id, project_id, 'revert',  auto_id, $now,       host FROM timeslice where id = ?
SQL

    $sth->execute($user_id);

    my $rows = $sth->rows;

    my $ret;
    if ($rows == 1) {
        my ($id, $flag, $rid, $pid, $host) = $sth->fetchrow_array;

        if ($flag eq $p{type} and defined $rid and $rid > 0) {
            eval {
                $sthi->execute($rid);
            };
            if ($@) {
                $dbh->rollback;
            } else {
                $dbh->commit;
		$ret = $rid;
            }
        } else {
            warn "Rollback";
        }
    } elsif ($rows == 0) {
        warn "Nothing to revert too." if ($Trasker::TTDB::debug);
    } else {
        die "Fatal Error: ", $rows;
    }
    $sth->finish();
    return $ret;
}

sub auto_revert_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        id => 0,
	host => 1,
	desktop => 0,
	name => 0,
	class => 0,
	title => 0,
	role => 0,
    });

    $self->revert(type => 'window', host => $p{host});  # We only want to revert it if we set it.
}

=item B<auto_get_project>

=cut

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

=item B<auto_set_project>

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

    my $ret;
    if (defined $auto) {
#        my $newproject = $self->project($auto->project_id);
#        my $curproject = $self->current_project();

        $ret = $self->set_current_project(project_id => $auto->project_id, temporary => 'window', auto_id => $auto->id, host => $p{host});
    } else {
        $ret = $self->revert(type => 'window', host => $p{host});
    }
    return $ret;
}

=item B<has_project>

=cut

sub has_project
{
    my $self = shift;
    my $dbh = get_dbh;
    my $id = $self->id() || die;
    my %p = validate(@_, {
        id => 0,
        project => {
           optional => 1,
           isa => [ qw( Trasker::TTDB::Project ) ],
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

=item B<add_task>

=cut

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
    my $now = 'now()';
    if (dbtype eq 'sqlite') {
        $now = q[datetime('now')];
    }

    if ($p{time}) {
        $st = $dbh->prepare("insert into notes (type, time, user_id, note) values (2, ?, ?, ?)");

        $st->execute($p{time}->mysql, $self->id(), $p{note}) or die $dbh->err_str();
    } else {
        $st = $dbh->prepare("insert into notes (type, time, user_id, note) values (2, $now, ?, ?)");

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

    my $sth;
    $sth = $dbh->prepare(<<SQL);
select
       sum(1)
  from timeslice ts
   left join timeslise te on ts.end_id = te.id
  where start_time < date(?) + 1
    and coalesce(te.start_time, datetime('now')) >= date(?)
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
           case when date(ts.start_time) >= date(?) + interval '1 day' then date(?) + interval '1 day' else te.start_time end,
           now())  -
           case when date(ts.start_time) < date(?) then date(?) else ts.start_time end) as time
  from timeslice ts
   left join timeslice te on te.id = ts.end_id
  where ts.start_time < date(?) + interval '1 day'
    and coalesce(te.start_time, now()) >= date(?)
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
    $ret->{projects} = [];
    $ret->{project} = {};

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
and (start_time < ? and (te.start_time >= ? or start_time is NULL))
EOP
        push(@args, ($p{date} + 1)->mysql);
        push(@args, ($p{date} + 0)->mysql);
    }
    my $sth = $dbh->prepare(<<SQL);
select ts.id, ts.user_id, ts.project_id, ts.start_time, te.start_time as end_time, ts.temporary, ts.auto_id, ts.revert_to, ts.host, coalesce(te.start_time - ts.start_time, now() - ts.start_time) as elapsed, ts.end_id
  from timeslice ts 
  left join timeslice te on ts.user_id = te.user_id and te.id = ts.end_id
 where ts.user_id = ?
   and (ts.start_time < ? and (te.start_time >= ? or ts.end_id is NULL))
 order by ts.start_time
 limit 10000
SQL
    eval {
      $sth->execute($self->id, @args);
    };
    if ($@) {
      $dbh->rollback();
      die;
    }

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
select ts.id,
       ts.start_time,
       te.start_time as end_time,
       ts.end_id,
       ts.temporary,
       ts.host,
       coalesce(te.start_time, now()) - ts.start_time as elapsed,
       ts.user_id,
       ts.auto_id,
       ts.revert_to,
       ts.project_id
  from timeslice ts 
    join timeslice te on ts.end_id = te.id
 where ts.id = ?
   and ts.user_id = ?
   for update;
SQL

    my $sthi = $dbh->prepare(<<SQL);
insert into timeslice
       (user_id, project_id, temporary, start_time, auto_id, revert_to, host, end_id)
 select user_id, project_id, 'split', start_time, auto_id, revert_to, host, id
     from timeslice where id = ?
SQL

    my $sth_st = $dbh->prepare(<<SQL);
update timeslice
   set start_time = ?
 where id = ?
SQL

    my $sth_et = $dbh->prepare(<<SQL);
update timeslice
   set end_id = ?
 where end_id = ? and id != ?
SQL
    my @timeslices = ($curid);
    eval {
	$sth_s->execute($curid, $self->id);		# Lock the timeslice being split

	die "Timeslice not found " . $curid . " " . $self->id unless $sth_s->rows;

	my $data = $sth_s->fetchrow_hashref();

	require Trasker::TTDB::TimeSlice;

	my $orig = Trasker::TTDB::TimeSlice->new(%$data);
        if (! scalar @times) {
            push @times, $orig->start_time + $orig->duration->divide();
	}
	my @splittimes = sort({ $b cmp $a } @times);

	foreach my $splittime (@splittimes) {
	    if ($splittime ge $orig->end_time or $splittime le $orig->start_time) {
		die "Time out of range " . $splittime->mysql();
	    }

	    #user_id, project_id, temporary, start_time, auto_id, revert_to, host
	    $sthi->execute($orig->id);

	    $sth_id->execute();

	    my $id = $sth_id->fetchrow();   # the new_id of the inserted entry

	    push @timeslices, $id;

	    my $t = $splittime->mysql;
	    $sth_et->execute($id, $curid, $id);  # update the old timeslice
	    $sth_st->execute($t, $curid);
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

