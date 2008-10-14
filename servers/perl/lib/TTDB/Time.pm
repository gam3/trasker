use strict;

## @file
## The Time object
##
#

## @class
# The Time object
#
# This object holds a timeslice
#
package TTDB::Time;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

use TTDB::DBI qw (get_dbh);

sub new
{
    my $class = shift;
    my $self = bless({ @_ }, $class);
    my $dbh = get_dbh();

    my %p = validate(@_, {
	id => 0,
    });

    my $sth = $dbh->prepare(<<SQL);
select *
  from timeslice
 where id = ?
SQL

    $sth->execute( $self->{id} );

    my $data = $sth->fetchrow_hashref();

    die Dumper($data, \%p) unless $data->{id};

    return bless { id => $data->{id}, data => $data }, $class;
}

sub id 
{
    my $self = shift;
    
    $self->{id};
}

sub time
{
    my $self = shift;
    if ((my @date) = ($self->{data}{time} =~ /(\d*):(\d*):(\d*)/)) {
warn " @date ";        
	return Date::Calc::MySQL->new([1], 0, 0, 0, @date);
    } else {
warn '0';        
        return Date::Calc::MySQL->new([1], 0, 0, 0, 0, 0, 0);
    }
}

sub elapsed
{
    my $self = shift;

    $self->{data}{time};
}

sub duration
{
    my $self = shift;

    $self->{data}{elapsed};
}

sub active
{
    my $self = shift;

    !!$self->{data}{end_time};
}

sub print
{
   my $self = shift;
   print $self->active ? 'a' : 'b';
}

use Date::Calc::MySQL;
use Date::ICal;

sub ical_start
{
    my $self = shift;

    my @date = split(/[-: ]/, $self->{data}{start_time});

    Date::ICal->new( year => $date[0],
                     month => $date[1], day => $date[2],
		     hour => $date[3], min => $date[4], sec => $date[5] )->ical;
}

sub ical_end
{
    my $self = shift;

    my @date = split(/[-: ]/, $self->{data}{end_time});

    Date::ICal->new( year => $date[0],
                     month => $date[1], day => $date[2],
		     hour => $date[3], min => $date[4], sec => $date[5] )->ical;
}

sub project_name
{
    my $self = shift;

    $self->{data}{name};
}

sub user_name
{
    my $self = shift;
    
    $self->{data}{user_name};
}

sub name
{
    my $self = shift;
    
    $self->{data}{name};
}

1;
__END__

=head1 NAME

TTDB::Time - Perl interface to the tasker timeslice table

=head1 SYNOPSIS

  use TTDB::Time;

  $user = TTDB::Time->new(user => 'bob', fullname => 'Robert Smith'):

  $user->create();

  $user = TTDB::Time->get(user => 'bob'):
  $user = TTDB::Time->get(id => 1):

=head1 DESCRIPTION

=head2 Constructors

=over

=item new()

=back

=head2 Methods

=over

=item id

Each timeslice has a unique I<id>.

=item active

=item duration

=item elapsed

=item ical_end

=item ical_start

=item name

=item print

=item project_name

=item time

=item user_name

=back

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

