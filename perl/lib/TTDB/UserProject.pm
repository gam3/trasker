use strict;
use warnings;

package TTDB::UserProject;

use TTDB::Projects;
use TTDB::User;
use TTDB::Project;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

sub new
{
    my $dbh = get_dbh;
    my $class = shift;
    my $self = bless { }, ref($class) || $class;

    my %p = validate(@_, {
        user => { optional => 1, isa => 'TTDB::User' },
        project => { optional => 1, isa => 'TTDB::Project' },
        project_id => { optional => 1 },
        user_id => { optional => 1 },
    });

    if (ref($class)) {
        $self->{user} = $class->{user};
        $self->{project} = $class->{project};
    }
    $self->{user} = $p{user} if $p{user};
    $self->{project} = $p{project} if $p{project};
    if (my $uid = $p{user_id}) {
        require TTDB::User;
        $self->{user} = TTDB::User->get(id => $uid);
    }
    if (my $pid = $p{project_id}) {
        require TTDB::Project;
        $self->{project} = TTDB::Project->get(id => $pid);
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

    my $entry = bless { longname => $longname, data => { project_pid => $p{parent}, name => $p{name}, } }, 'TTDB::Project'; 

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
    time => 0,
    data => {},
};

sub get_time
{
    my $self = shift;
    my $id = $self->project->id();

    if ($get_time->{'time'} && time - $get_time->{'time'} < 5) {
	return $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0,0,0);
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

    while (my $row = $sth->fetchrow_arrayref) {
	my $time = $row->[1];
	my $ntime = Date::Calc::Object->new([1], 0, 0, 0, split(':', $time));
	$get_time->{data}->{$row->[0]} = $ntime;
    }
    $get_time->{time} = time();
    $get_time->{data}->{$id} || Date::Calc::Object->new([1], 0, 0, 0, 0, 0, 0);
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

    map({ TTDB::UserProject->new(user => $self->user, project => $_); } $self->project->children);
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

sub alltime
{
    my $self = shift;

    my $time = $self->_alltime();

    sprintf("%2d:%02d:%02d", $time->normalize()->time());
}

sub name {
    shift->project->name;
}

sub longname {
    shift->project->longname;
}

sub add_note
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
        note => 1,
    });
    my $st = $dbh->prepare("insert into notes (type, time, user_id, project_id, note) values (1, now(), ?, ?, ?)");

    eval {
	$st->execute($self->user->id(), $self->project->id(), $p{note}) or die $dbh->errstr();
    }; if ($@) {
	$dbh->rollback;
	die $@;
    } else {
warn "Added note";
	$dbh->commit or die $dbh->errstr();
    }

    $self;
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
    
    TTDB::Projects::flush();

    $self->new(project_id => $id);
}

1;
__END__
