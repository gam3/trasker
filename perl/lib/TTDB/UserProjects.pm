use strict;

package TTDB::UserProjects;

use TTDB::Projects;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

our $dir = "/home/gam3/.tasker/";

our $data;

sub new
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
        user => { isa => 'TTDB::User' },
    });

    my $user_id = $p{user}->id;

    my $sth = $dbh->prepare(<<SQL);
select project_id project_id,
       user_id user_id,
       'eof'
  from user_project
  where user_id = ?
SQL

    $sth->execute($user_id);

    my $data_hash = {};
    my $head = [];

    while (my $data = $sth->fetchrow_hashref()) {
        my $id = $data->{project_id};
	$user_id = $data->{user_id};

	# Merge data in case child gets set before data is seen.
	my $entry = bless { data => $data, child => $data_hash->{$id}{child} }, 'TTDB::Project'; 

	$data_hash->{$id} = $entry; 
	if (my $parent_id = $data->{project_pid}) {
	    # the parent may not me definded yet
	    $data_hash->{$parent_id}{child}{$id} = 1;
	} else {
	    push(@$head, $id);
	}
    }
    $data = bless { data => $data_hash, head => $head }, $class;
}

sub user_id
{
    my $self = shift;

    die "Use ProjectUser";
}

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

    die "use ProjectUser";

}

1;
