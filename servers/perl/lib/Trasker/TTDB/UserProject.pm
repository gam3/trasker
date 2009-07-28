use strict;
use warnings;

## @file
## The UserProject object
##
#

## @class
# A tuple of User and Project
#
# This Object holds User and Project Objects and has a number of
# helper methods that work on those objects.
#
package Trasker::TTDB::UserProject;

use Trasker::TTDB::Projects;
use Trasker::TTDB::User;
use Trasker::TTDB::Project;

use Carp qw (croak);

use Trasker::TTDB::DBI qw (get_dbh dbi_setup);

use Trasker::Date;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

sub new
{
    my $dbh = get_dbh;
    my $class = shift;
    my $self = bless { }, ref($class) || $class;

    my %p = validate(@_, {
        user => { optional => 1, isa => 'Trasker::TTDB::User' },
        project => { optional => 1, isa => 'Trasker::TTDB::Project' },
        project_id => { optional => 1 },
        user_id => { optional => 1 },
    });

    if (ref($class)) {
        $self->{user} = $class->{user};
        $self->{project} = $class->{project};
    }
    $self->{user} = $p{user} if $p{user};
    if ($p{user}) {
	$self->{user} = $p{user};
    } elsif (my $uid = $p{user_id}) {
        require Trasker::TTDB::User;
        $self->{user} = Trasker::TTDB::User->get(id => $uid);
    }
    unless ($self->{user}) {
        croak "Need a user";
    }

    if ($p{project}) {
	$self->{project} = $p{project};
    } elsif (my $pid = $p{project_id}) {
        require Trasker::TTDB::Project;
        $self->{project} = Trasker::TTDB::Project->get(id => $pid);
    }
    unless ($self->{project}) {
        die "Need a project";
    }
    return $self;
}

sub user_id
{
    my $self = shift;

    die "Use ProjectUser";
}

sub project
{
    my $self = shift;
    $self->{project};
}

sub user
{
    my $self = shift;
    $self->{user};
}

sub active
{
    my $self = shift;
    my $dbh = get_dbh('select');
    my $user_id = $self->user->id;
    my $project_id = $self->project->id;

    if (defined $self->project->user_id
        && $self->project->user_id == $self->{user}->id()
       )
    {
        return 1;
    }

    my $sth = $dbh->prepare(<<SQL);
select count(*)
  from user_project
 where user_id = ? and project_id = ?
SQL

    $sth->execute($user_id, $project_id);

    my $ret = $sth->fetchrow();

    return $ret;
}

sub set
{
    my $self = shift;
    my $dbh = get_dbh();
    my $user_id = $self->user->id;
    my $project_id = $self->project->id;

    my $sth = $dbh->prepare(<<SQL);
insert into user_project (user_id, project_id) values (?, ?)
SQL

    $sth->execute($user_id, $project_id);
    $self;
}

sub clear
{
    my $self = shift;
    my $dbh = get_dbh();
    my $user_id = $self->user->id;
    my $project_id = $self->project->id;

    my $sth = $dbh->prepare(<<SQL);
delete from user_project
 where user_id = ? and project_id = ?
SQL

    $sth->execute($user_id, $project_id);
    $self;
}

*unset = \&clear;

sub get_entry
{
    my $self = shift;
    my $id = shift;

    return $self->{data}->{$id} || die Dumper($self->[0])." No entry found ($id).";
}

sub subentries
{
    my $self = shift;
    my $hash = $self->{data};
    my $list = $self->{head};
    my $data = shift;
    my $in = shift;
    my @ret;
    for my $root (keys %$data) {
        my $entry = $hash->{$root};
	$entry->{long_name} =  $in . ':' . $entry->name();
        push @ret, $entry;
	if (my $x = $entry->child()) {
	    push @ret, $self->subentries($x, $in . ':' . $entry->name());
	}
    }
    @ret;
}

sub entries
{
    my $self = shift;
    my $hash = $self->{data};
    my $list = $self->{head};
    my @ret;

    for my $root (@$list) {
        my $entry = $hash->{$root};
	$entry->{long_name} = $entry->{project_name};
        push @ret, $entry;
	if (my $x = $entry->child()) {
	    push @ret, $self->subentries($x, $entry->name());
	}
    }
    @ret;
}

sub add
{
    my $self = shift;
    my %p = validate(@_, {
	parent => 1,
	name => 1,
	description => 0,
    });
    my $parent = $self->get_entry($p{parent});

    my $longname = $parent->longname . '::' . $p{name};

    my $entry = bless { longname => $longname, data => { project_pid => $p{parent}, name => $p{name}, } }, 'Trasker::TTDB::Project';

    $entry;
}

sub start
{
    my $self = shift;
    my $id = shift;
    my $dbh = get_dbh;

    $self->user->set_current_project(
        project => $self->project,
	host => undef,
    );
    return $self;
}

our $get_time = {
    time => {},
    data => {},
};

sub week
{
    require Trasker::TTDB::TimeSpan;
    my $self = shift;
    my %p = validate(@_, {
        date => { isa => 'Date::Calc' },
        all => 0,
    });
    my $date = $p{date};

    my $start = Trasker::Date->new($date->date);
    my $end = Trasker::Date->new(($date + 7)->date);

    my @project_ids = ( $self->project->id );
    if ($p{all}) {
	push(@project_ids, map({ $_->id; } $self->project->children));
    }

    Trasker::TTDB::TimeSpan->get(
        start_time => $start,
        end_time => $end,
	uids => [ $self->user->id ],
	pids => [ @project_ids ],
    );
}

sub day
{
    require Trasker::TTDB::TimeSpan;
    my $self = shift;
    my %p = validate(@_, {
        date => { isa => 'Date::Calc' },
        all => 0,
    });
    my $date = $p{date};


    my $start = Trasker::Date->new($date->date);
    my $end = Trasker::Date->new(($date + 1)->date);

    my @project_ids = ( $self->project->id );

    if ($p{all}) {
	push(@project_ids, map({ $_->id; } $self->project->children));
    }

    my $dur = Trasker::TTDB::TimeSpan->get(
        start_time => $start,
        end_time => $end,
	uids => [ $self->user->id ],
	pids => [ @project_ids ],
    );
    my $duration = Trasker::Date->new($dur->{data}{time});
    my $max = Trasker::Date->new(1, 0, 0, 0);

die 'bad duration' if $duration > $max;
    $dur;
}

sub get_time
{
    my $self = shift;
    my $id = $self->project->id();

    if ($get_time->{'time'} && time - $get_time->{'time'} < 5) {
	return $get_time->{data}{$id} || Trasker::Date->new([1], 0, 0, 0, 0,0,0);
    }

    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL) or die $dbh->err_str();
select project_id,
  SUM(
     coalesce(end_time, now()) - case when start_time >= 'today' then start_time else 'today' end
  )
  from timeslice
 where date(coalesce(end_time, 'today')) = 'today'
   and user_id = ?
 group by project_id
SQL

    $sth->execute($self->user->id);

    $dbh->commit;

    $get_time->{data} = {};
    $get_time->{time} = time();
    while (my $row = $sth->fetchrow_arrayref) {
	my $time = $row->[1];
	my $ntime = Trasker::Date->new([1], 0, 0, 0, split(':', $time));
	if (defined($row->[0])) {
	    $get_time->{data}{$row->[0]} = $ntime;
	} else {
	    $get_time->{data}{0} = $ntime;
	}
    }
    $get_time->{data}{$id} || Trasker::Date->new([1], 0, 0, 0, 0, 0, 0);
}

sub time
{
    my $self = shift;
    my $time = $self->get_time();

    sprintf("%2d:%02d:%02d", $time->normalize()->time());
}

sub children
{
    my $self = shift;

    map({ Trasker::TTDB::UserProject->new(user => $self->user, project => $_); } $self->project->children);
}

sub get_alltime
{
    my $self = shift;
    my $time = $self->get_time();
    foreach my $child ($self->children) {
        $time += $child->get_alltime;
    }
    $time;
}

sub alltime
{
    my $self = shift;

    my $time = $self->get_alltime();

    sprintf("%2d:%02d:%02d", $time->normalize()->time());
}

sub name {
    croak("use ->project->name");
    shift->project->name;
}

sub longname {
die "use ->project->longname";
    shift->project->longname;
}

sub add_note
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        time => {
	   optional => 1,
	   isa => 'Date::Calc',
	},
        note => 1,
        type => { default => 1 },
    });

    my $st;

    my $type = $p{type} || 1;

    my $st_id = $dbh->prepare(qq/select nextval('note_id_seq')/);

    my @extra = ();
    if ($p{time}) {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, ?)");
	push @extra, $p{time}->mysql;
    } else {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, now())");
    }

    require Trasker::TTDB::Note;
    my $note = Trasker::TTDB::Note->new(
	%p,
        user => $self->user,
        project => $self->project,
    );

    eval {
	$st->execute($type, $self->user->id(), $self->project->id(), $p{note}, @extra);
    }; if ($@) {
	$dbh->rollback;
	die $@;
    } else {
	$dbh->commit or die $dbh->errstr();
    }

    $note;
}

sub get_notes
{
    my $self = shift;
    require Trasker::TTDB::Notes;
    my %p = validate(@_, {
    });

    Trasker::TTDB::Notes->new(
	user_ids => [ $self->user->id ],
	project_ids => [ $self->project->id ],
    )->entries;
}

sub get_selects
{
    my $self = shift;
    require Trasker::TTDB::Autos;
    my %p = validate(@_, {
    });

    Trasker::TTDB::Autos->new(
	user_ids => [ $self->user->id ],
	project_ids => [ $self->project->id ],
    )->entries;
}

sub add_task
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        name => 1,
        description => 1,
    });
    my $st_id = $dbh->prepare(qq/select nextval('project_id_seq')/);
    my $st = $dbh->prepare("insert into project (id, user_id, parent_id, name, description) values (?, ?, ?, ?, ?)");

    my $id;
    eval {
	$st_id->execute();
	$id = $st_id->fetchrow();
	$st->execute($id, $self->user->id(), $self->project->id(), $p{name}, $p{description}) or die $dbh->errstr();
    };
    if ($@) {
        $dbh->rollback;
	die $@;
    } else {
        $dbh->commit;
    }

    Trasker::TTDB::Projects::flush();

    $self->new(project_id => $id);
}

sub depth
{
    my $p = shift->project;

    $p->depth;
}

use Data::Dumper;

sub get_all_auto
{
    my $self = shift;
    my $dbh = get_dbh;

    my $st = $dbh->prepare(<<SQL);
select * from auto where user_id = ? and project_id = ?
SQL
    $st->execute($self->user->id, $self->project->id);
    my @ret;

    require Trasker::TTDB::Auto;

    while (my $row = $st->fetchrow_hashref()) {
        push @ret, Trasker::TTDB::Auto->new(%$row);
    }
    @ret;
}

sub new_auto
{
    my $self = shift;
    require Trasker::TTDB::Auto;

    my $auto = Trasker::TTDB::Auto->new(
        user_id => $self->user->id,
        project_id => $self->project->id,
	@_,
    );

    $auto = Trasker::TTDB::Auto->create(
        user_id => $self->user->id,
        project_id => $self->project->id,
	@_,
    );

    $auto;
}

sub rate
{
    20.00;
}

1;
__END__

=head1 NAME

Trasker::TTDB::UserProject - Perl interface to the tasker auto table

=head1 SYNOPSIS

  use Trasker::TTDB::UserProject;

  $up = Trasker::TTDB::UserProject->new(user => I<user>, project => I<project>);

  $up = Trasker::TTDB::UserProject->new(
      project_id => I<project id>,
      user_id => I<user id>,
  );

  $up = Trasker::TTDB::UserProject->get(user => $user, role => 'bob'):

=head1 DESCRIPTION

This is a container that holds a User Object and a Project Object.

=head2 Constructor

=over

=item new

This does not put the object into the database.

=item get

Get an object from the database.

=back

=head2 Methods

=over

=item  id

return the I<id> of the object.

=item  user_id

return the I<user_id> for the object.

=item  project_id

return the I<project_id> for the object.

=item delete

This will delete the object from the database.

=item active

The project is active for the user

=item add

=item add_note

=item add_task

=item alltime

=item children

=item clear

=item depth

=item entries

=item get_all_auto

=item get_alltime

=item get_entry

=item get_time

=item longname

=item name

=item new_auto

=item project

=item set

=item start

Make this the current project for the user.

=item subentries

=item time

=item unset

=item user

=item day

Get time for given date

=item rate

The rate of pay for the project/user

=item get_notes

Get the notes for a user/project

=item get_selects

Get the selects for a user/project

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

