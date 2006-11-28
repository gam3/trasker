use strict;

use Test::More tests => 4;

BEGIN {
    use_ok('TTDB::Projects');
};

my $projects = TTDB::Projects::get();

my $count;

for my $project ($projects->entries()) {
    $count++;
}

ok($count == 11, 'count');

ok($projects->get_entry(1), 'get_entry');

is($projects->get_entry(11)->longname, 'Test:bill:bob', 'longname');

$projects->flush();


