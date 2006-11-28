use strict;

use Test::More tests => 3;
use Test::Exception;

use Date::Calc::MySQL;

BEGIN {
    use_ok('TTDB::Notes');
};

my $notes = TTDB::Notes->new();

$notes->entries;

$notes = TTDB::Notes->new(today => 1);

is(scalar $notes->entries, 1);

$notes = TTDB::Notes->new(date => Date::Calc::MySQL->new(2006, 1, 1));

is(scalar $notes->entries, 1);
