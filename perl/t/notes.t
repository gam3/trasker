use strict;

use Test::More tests => 4;
use Test::Exception;

use Date::Calc::MySQL;

BEGIN {
    use_ok('TTDB::Notes');
};

my $notes = TTDB::Notes->new();

$notes->entries;

$notes = TTDB::Notes->new(today => 1);

is(scalar $notes->entries, 0, 'today');

$notes = TTDB::Notes->new(date => Date::Calc::MySQL->new(2006, 1, 1));

is(scalar $notes->entries, 0, '2006-01-01');

$notes = TTDB::Notes->new(date => Date::Calc::MySQL->new(2006, 1, 02));

is(scalar $notes->entries, 0, '2006-01-02');
