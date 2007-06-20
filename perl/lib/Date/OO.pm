use strict;
use warnings;

package TurboSchedule::Date;

use base qw(Date::Calc::Object);

use Carp qw (croak);

=pod

=head1 NAME

Date::OO - Interface to CGI or mod_perl

=head1 SYNOPSIS

This package overloads Date::Calc::Object for the TurboSchedule package.

=head1 DESCRIPTION

=over

=item new

Extend Date::Calc::Object to take "2005-01-31" as argument.

=cut

sub new
{
    my $class = shift;
    my $ret =
    eval {
        if (@_ == 1) {
            my $data = shift;
            if ($data =~ /^\+/) {
                my ($x, @date) = split(':', $data);
                die "Unknown duration type ($x)" if $x + 0;
                return $class->SUPER::new([1], @date);
            } else {
                my @date = split('-', $data);
                Date::Calc::check_date(@date);
                return $class->SUPER::new(@date);
            }
        } else {
            $class->SUPER::new(@_);
        }
    }; die __PACKAGE__."::new(@_:" . @_ . ") [$@] ", join(' ', caller)  if $@;
    $ret;
}

1;
