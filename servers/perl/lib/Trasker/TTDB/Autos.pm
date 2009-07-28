use strict;
use warnings;

## @file
## The Autos object
##
#

## @class
# A list of notes
#
# This Object simply holds a list of Note objects
#
package Trasker::TTDB::Autos;

use Trasker::TTDB::Auto;

use Params::Validate qw (validate ARRAYREF);

use Trasker::TTDB::DBI qw (get_dbh);

sub new
{
    my $class = shift;

    my %p = validate(@_, {
	project_ids => {
	    optional => 1,
	    type => ARRAYREF,
	},
	user_ids => {
	    optional => 1,
	    type => ARRAYREF,
	},
    });

    bless {%p}, $class;
}

sub entries
{
    my $self = shift;

    my $dbi = get_dbh;
    my @ret;
    my @args = ();

    my $sql = "select * from auto where 1 = 1";
    my @projects;
    my @users;

    if (my $a = $self->{project_ids}) {
        $sql .= sprintf(' and project_id in (%s)', join(', ', map({'?'} @{$a})));
	push(@args, @$a);
    }

    if (my $a = $self->{user_ids}) {
        $sql .= sprintf(' and user_id in (%s)', join(', ', map({'?'} @{$a})));
	push(@args, @$a);
    }

    $sql .= ' order by presidence';
    $sql .= ' limit 100';

    my $sth = $dbi->prepare($sql);

    $sth->execute(@args);
    require Trasker::TTDB::Note;

    @ret = map({Trasker::TTDB::Auto->new(%{$_})} @{$sth->fetchall_arrayref({})});
    @ret;
}

1;
__END__

=head1 NAME

Trasker::TTDB::Autos - Perl interface to the tasker notes table

=head1 SYNOPSIS

  use Trasker::TTDB::Autos;

  $auto = Trasker::TTDB::Autos->new(user => $user)->entries;

=head1 DESCRIPTION

=head2 Constructor

=over

=item new

This does not put the object into the database.

=item get

Get an object from the database.

=item create

This will create a auto entry, if one does not exist.

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

=item entries

This will delete the object from the database.

=back

=head1 SEE ALSO

L<Trasker::TTDB::Note>

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

