use strict;

package TTDB::Project;
use TTDB::DBI qw (get_dbh);

use TTDB::Projects;
use Date::Calc::Object;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

sub new
{
    my $class = shift;

    my %p = validate(@_, {
        name => 1,
	parent => {
	   optional => 1,
	   isa => [ qw(  TTDB::Project ) ],
	},
	user => {
	   optional => 1,
	   isa => [ qw(  TTDB::User ) ],
	},
    });

    my $self = bless({ hide => 'N', %p }, $class);

    return $self;
}

sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
	id => 0,
	child => {
	   optional => 1,
	   isa => [ qw(  TTDB::Project ) ],
	},
	user => {
	   optional => 1,
	   isa => [ qw(  TTDB::User ) ],
	},
	name => 0,
	root => 0,
    });

    my $id = $p{id};

    my $projects = TTDB::Projects::get((user => $p{user}) x!! $p{user});

    if (my $name = $p{name}) {
        for my $k (keys %{$projects->{data}}) {
            my $pn = $projects->{data}{$k}{name};
	    next unless $name eq $pn;
	    $p{id} = $k;
	    last;
	}
	die "No Project $p{name}" unless $p{id};
    }

    my $project = $projects->{data}{$p{id}};

    return $project || die "No project $p{id}.";
}

sub create
{
    my $self = shift;

    my %p = validate(@_, {
    });

    die 'All ready created' if $self->{id};

    my $dbh = get_dbh;
    my $pid = $self->parent_id;
    my $name = $self->name;
    my $user_id = $self->user_id;
    my $hidden = $self->hidden;
    my $description = $self->description;

    my $st_id = $dbh->prepare('select LAST_INSERT_ID()');

    my $sth = $dbh->prepare(<<EOP);
insert into project (parent_id, name, user_id, hide, description) values (?, ?, ?, ?, ?);
EOP
    $sth->execute($pid, $name, $user_id, $hidden, $description);

    $st_id->execute();
    my $id = $st_id->fetchrow();
    $self->{id} = $id;
    
    TTDB::Projects::flush();

    $self;
}

sub update
{
    my $self = shift;
    my $dbh = get_dbh;
    my @data;
    my @q;
    my %p = validate(@_, {
        parent_id => 0,
        name => 0,
    });

    my $sql = 'update project set ';
    if ($p{parent_id}) {
	die 'Circular reference' if $self->{child}->{$p{parent_id}};
    }

    for my $key (keys %p) {
	push(@q, $key);
	push(@data, $p{$key});
    }
    $sql .= join(', ', map({"$_ = ?"} @q));
    $sql .= " where id = ?";

    my $st = $dbh->prepare($sql) or die $dbh->errstr;

    $st->execute(@data, $self->id);
}

sub _create_child
{
    my $self = shift;
    my %p = validate(@_, {
        name => 1,
    });

    $self->new(parent => $self, name => $p{name})->create;
}

sub parent
{
   my $self = shift;

   return $self->{parent} if defined $self->{parent};

   if (my $id = $self->{parent_id}) {
       $self->{parent} = TTDB::Project->get(id => $id, child => $self);
   }
   $self->{parent};
}

sub parent_id
{
    my $self = shift;

    my $p = $self->parent;

    if ($p) {
        return $p->id;
    } else {
        return 0;
    }
}

sub longname
{
   my $self = shift;

   return $self->{long_name} if defined $self->{long_name};

   my @name;
   my $parent = $self;
   while ($parent) {
       push(@name, $parent->name);
       $parent = $parent->parent;
   }
   
   $self->{long_name} = join(':', reverse @name);
}

sub name
{
   my $self = shift;

   $self->{name};
}

sub id
{
   my $self = shift;

   $self->{id} || die "no id for project";
}

sub hidden
{
   my $self = shift;

   $self->{hide};
}

sub description
{
   my $self = shift;

   $self->{description};
}

sub users
{
    my $self = shift;
    require TTDB::Users;

    TTDB::Users->new(project_id => $self->id());
}

sub child
{
   my $self = shift;
   $self->{child};
}

sub add_child
{
   my $self = shift;
   my $child_id = shift;
   push(@{$self->{child}}, $child_id);

   $self;
}

our $get_time = {
    time => 0,
    data => {},
};

sub get_time
{
    my $self = shift;
    my $id = $self->id();

    if (time - $get_time->{time} < 1) {
	return $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0,0,0);
    }

    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select project_id,
  SEC_TO_TIME(
   SUM(
    TIME_TO_SEC(
     timediff(now(), if(start_time >= date(now()), start_time, concat(date(now()), ' 00:00:00')))
    )
   )
  )
  from timeslice
 where addtime(start_time, timediff(ifnull(end_time, now()), start_time)) >= date(now())
 group by project_id
SQL

    $sth->execute();

    while (my $row = $sth->fetchrow_arrayref) {
	my $time = $row->[1];
	my $ntime = Date::Calc::Object->new([1], 0, 0, 0, split(':', $time));
	$get_time->{data}->{$row->[0]} = $ntime;
    }
    $get_time->{time} = time();
    $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0, 0, 0);
}

sub _alltime
{
    my $self = shift;
    my $time = $self->get_time();
    foreach my $child ($self->children) {
        $time += $child->_alltime;
    }
    $time;
}

sub get_times
{
    my $self = shift;
    my $id = $self->id();

    if (time - $get_time->{time} < 1) {
	return $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0,0,0);
    }

    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select project_id, 
  *
  from timeslice 
 where date(start_time) <= date(now())
   and (date(addtime(start_time, elapsed)) >= date(now())
    or elapsed is null)
 group by project_id;
SQL

    $sth->execute();

    while (my $row = $sth->fetchrow_arrayref) {
	my $time = $row->[1];
	my $ntime = Date::Calc::Object->new([1], 0, 0, 0, split(':', $time));
	$get_time->{data}->{$row->[0]} = $ntime;
    }
    $get_time->{time} = time();
    $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0, 0, 0);
}

sub children
{
    my $self = shift;

    map({ TTDB::Project->get(id => $_) } keys %{$self->{child}});
}

sub time
{
    my $self = shift;
    my $time = $self->get_time();

    sprintf("%2d:%02d:%02d", $time->normalize()->time());
}

sub alltime
{
    my $self = shift;

    my $time = $self->_alltime();

    sprintf("%2d:%02d:%02d", $time->normalize()->time());
}

sub is_task
{
    my $self = shift;
    $self->{data}{user_id};
}

sub user_id
{
    my $self = shift;

    $self->{user_id};
}

1;

__END__

=head1 NAME

TTDB::Project - Perl interface to the tasker project data

=head1 SYNOPSIS

  use TTDB::Project;

  $user = TTDB::Project(id => 1):

=head2 Constructor

=over

=item new

=item get

=item create

=back

=head2 Methods

=over

=item  id
=item  parent
=item  parent_id
=item  longname
=item  name
=item  id
=item  users
=item  child
=item  add_child
=item  get_time
=item  get_times

=item  hidden

=item  description

=item  children

returns a list of projects that are children of the project.

=item  time

Return the time this project has been active during the current day.

=item  alltime

Return the time this project and all its children have been active during the current day.

=item  is_task

return true if this is task rather than a project.

=item user_id

If a task then this returns the user_id for the task.

=item update

make changes to the object and store them in the database.

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

