use strict;

package TTDB::Projects;

use TTDB::Project;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Carp qw (croak);

our $dir = "/home/gam3/.tasker/";

our $data;

sub flush
{
    $data = undef;
}

sub get
{
    my $class = shift || croak "Need Class";

    my $data = $class->new(@_);

    $data;
}

sub new
{
    my $dbh = get_dbh('read');
    my $class = shift;

    my %p = validate(@_, {
	user => {
	   optional => 1,
	   isa => [ qw(  TTDB::User ) ],
	},
    });

    my $sth = $dbh->prepare(<<SQL) or die $dbh->err_str;
select project.name as name,
       project.id as id,
       project.parent_id as parent_id,
       project.user_id as user_id,
       project.description as description,
       'eof'
  from project
SQL

    $sth->execute() or die;

    my $data_hash = {};
    my $head = [];

    my $user_id;
    while (my $data = $sth->fetchrow_hashref()) {
        my $id = $data->{id};
	$user_id = $data->{user_id};

	# Merge data in case child gets set before data is seen.
	my $entry = bless { %$data, child => $data_hash->{$id}{child} }, 'TTDB::Project'; 

	$data_hash->{$id} = $entry; 
	if (my $parent_id = $data->{parent_id}) {
	    # the parent may not me definded yet
	    $data_hash->{$parent_id}{child}{$id} = 1;
	} else {
	    push(@$head, $id);
	}
    }

    $data = bless { data => $data_hash, head => $head }, $class;
}

sub get_entry
{
    my $self = shift;
    my $id = shift;
use Data::Dumper;
    return $self->{data}->{$id} || die Dumper($self)." No entry found ($id).";
}

sub _subentries
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
	    push @ret, $self->_subentries($x, $in . ':' . $entry->name());
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
	    push @ret, $self->_subentries($x, $entry->name());
	}
    }
    @ret;
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

=back

=head2 Methods

=over

=item get_entry

=item entries

=item get

=item flush

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

