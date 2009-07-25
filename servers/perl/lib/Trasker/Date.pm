use strict;
use warnings;

package Tasker::Date;

use Date::Calc::Object;

use base "Date::Calc::Object";

use POSIX qw();

=head1 NAME

Tasker::Date - The time and date function for Tasker

=head1 SYNOPSIS

  use Tasker::Date;

  $date = Tasker::Date->today();

=head2 Constructors

=head3 today($)

=over

return todays date.

=back

=cut

sub today
{
    shift->SUPER::today(@_);
}

sub new
{
    my $class = shift;
    my @data = @_;

    if (@_ == 1) {
        my $string = $data[0];
	if ($string =~ /-/) {
	    @data = split(/[- :]/, $string);
	} else {
            my @x;
	    if (my ($d, $x) = ($string =~ /(\d) days (.*)/)) {
                push(@x, 0, 0, $d);
                $string = $x;
	    } else {
                push(@x, 0, 0, 0);
            }
	    push(@x, split(/[:]/, $string));
	    @data = ([1], @x);
	}
    } else {

    }

    $class->SUPER::new(@data);
}

sub as_hours
{
    my $self = shift;
    my $format = shift || '%.1f';

    die 'Not a delta' unless ($self->is_delta);

    my $x = $self->[6] / 3600;
    $x += $self->[5] / 60;
    $x += $self->[4];

    if (my $day = $self->[3]) {
        $x += $day * 24; 
    }

    die Dumper $self if $self->[2];
    die Dumper $self if $self->[1];

    sprintf($format, $x);
}

sub mysql
{
    my $self = shift;
    my $ref = '';
    if ($self->is_delta) {
        $self->normalize();

	$ref = int(abs($self));

        $ref .= sprintf(" %d:%d:%d", $self->time);
    } else {
	$ref = join('-', $self->date);
	if ($self->is_long) {
	    $ref .= ' ' . join(':', $self->time);
	}
    }
    return $ref;
}

sub strftime
{
    my $self = shift;
    my $format = shift or die "Need a format";
   
    POSIX::strftime($format, localtime(Date::Calc::Mktime($self->datetime)));
}

1;

__END__
