use strict;

use Test::More tests => 4;
use Test::Exception;

use Trasker::Date;

BEGIN {
    use_ok('Trasker::TTDB::Notes');
};

my $notes = Trasker::TTDB::Notes->new();

$notes->entries;

$notes = Trasker::TTDB::Notes->new(today => 1);

is(scalar $notes->entries, 3, 'today');

$notes = Trasker::TTDB::Notes->new(date => Trasker::Date->new(2006, 1, 1));

is(scalar $notes->entries, 0, '2006-01-01');

$notes = Trasker::TTDB::Notes->new(date => Trasker::Date->new(2006, 1, 2));

is(scalar $notes->entries, 0, '2006-01-02');


