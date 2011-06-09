use strict;
use warnings;

package Trasker::Date;

use Date::Calc::Object;
use Carp;

$Carp::Internal{ (__PACKAGE__) }++;

use base "Date::Calc::Object";

use POSIX qw();

=head1 NAME

Trasker::Date - The time and date function for Trasker

=head1 SYNOPSIS

  use Trasker::Date;

  $date = Trasker::Date->today();

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
	if ($string =~ /T/) {
#    my ($d, $t) = split(/T/, $string);
	    @data = split(/[-T:]/, $string);
	} elsif ($string =~ /-/) {
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

    my $ret =
    eval {
	$class->SUPER::new(@data);
    }; if ($@) {
	croak($@);
    }
    $ret;
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

sub replace_time
{
    my $self = shift;
    my @time;
    if (@_ == 1) {
        my @temp = split(/:/, shift);
	while (@temp < 3) {
	    push(@temp, "0");
	}
	@time = map({ $_ + 0 } @temp);
    } elsif (@_ == 3) {
	@time = @_;
    } else {
        die "Bad input to replace_time";
    }

    ref($self)->new($self->date, @time)->normalize;
}

sub divide
{
    my $self = shift;
    my $c = $self->clone;

    die 'Not a delta' unless ($self->is_delta);

    if (@$c == 7) {
	if ($c->[3] & 1) {
	    $c->[3] -= 1;
	    $c->[4] += 24;
	}
	if ($c->[4] & 1) {
	    $c->[4] -= 1;
	    $c->[5] += 60;
	}
	if ($c->[5] & 1) {
	    $c->[5] -= 1;
	    $c->[6] += 60;
	}
	if ($c->[6] & 1) {
	    $c->[6] -= 1;
	}
	$c->[3] /= 2;
	$c->[4] /= 2;
	$c->[5] /= 2;
	$c->[6] /= 2;
    } else {
	die "bob";
    }

    return $c;
}

1;

__END__
