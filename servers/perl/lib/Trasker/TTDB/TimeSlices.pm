use strict;

## @file
# The TimeSlices package
#

## @class
# A list of User objects
#
# This is simply a list of User Objects
#
package Trasker::TTDB::TimeSlices;

use Trasker::TTDB::DBI qw (get_dbh);
use Trasker::TTDB::TimeSlice;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

sub new
{
    my $dbh = get_dbh;
    my $class = shift;

    return bless { data => \@_ }, $class;
}

sub entries
{
    my $self = shift;

    @{$self->{data}};
}

1;
__END__

