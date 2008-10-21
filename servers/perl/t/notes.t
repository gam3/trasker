use strict;

use Test::More tests => 4;
use Test::Exception;

use Tasker::Date;

BEGIN {
    use_ok('Tasker::TTDB::Notes');
};

my $notes = Tasker::TTDB::Notes->new();

$notes->entries;

$notes = Tasker::TTDB::Notes->new(today => 1);

is(scalar $notes->entries, 3, 'today');

$notes = Tasker::TTDB::Notes->new(date => Tasker::Date->new(2006, 1, 1));

is(scalar $notes->entries, 0, '2006-01-01');

$notes = Tasker::TTDB::Notes->new(date => Tasker::Date->new(2006, 1, 2));

is(scalar $notes->entries, 0, '2006-01-02');


