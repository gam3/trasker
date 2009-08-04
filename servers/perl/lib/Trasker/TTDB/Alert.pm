use strict;
## @file
# The Alert object
#

## @class
# The Alert Object.
#
# A user is normally a human, but can be any object that can be in only
# one place at one time.
#
package Trasker::TTDB::Alert;

use Trasker::TTDB::DBI qw (get_dbh);

our $VERSION = '0.001';

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use Carp qw (croak);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        id => 0,
	start_time => 1,
	end_time => 1,
	alert_type_id => 0,
        title => 1,
        description => 1,
	recurs_id => 0,
	updated => 0,
    });

    return bless { %p }, $class;
}

sub create
{
    my $class = shift;

    my %p = validate(@_, {
        id => 0,
	start_time => 1,
	end_time => 0,
        title => 1,
        description => 1,
	alert_type_id => 0,
	recurs_id => 0,
	updated => 0,
    });

    my $self;
    my $dbh = get_dbh;

    my $sth_id;
    if (0) {
        $sth_id = $dbh->prepare('select LAST_INSERT_ID()');
    } else {
        $sth_id = $dbh->prepare("select currval('users_id_seq')");
    }
    my $sth = $dbh->prepare(<<SQL);
insert into alert (name, fullname) values (?, ?)
SQL

#    $sth->execute($self->{name}, $self->{fullname});
#    $sth_id->execute();
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
    });

    my $data;

    die "No Alert" unless $data;

    return bless { %$data }, $class;
}

sub id
{
    my $self = shift;

    $self->{id} || die "No id";
}


1;
__END__

=head1 NAME

Trasker::TTDB::Alert - Perl interface to the tasker alert

=head1 SYNOPSIS

  use Trasker::TTDB::Alert;

  $alert = Trasker::TTDB::User->new(id => I<id>):

=head2 Constructor

=over

=item new

This creates a user object.  Use I<create> to make this object
preminate.

=item get

I<get> returns an alert object that describes the alert requested.

=back

=head2 Methods

=over

=item  id

=item  userid

=item  name

This is an alias to the userid function

=item fullname

=item projects

=item project

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

=item days

Return information about a give range of days for the user.

=item get_timeslices_for_day

=item times

Get the timeslices for a given period

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut
