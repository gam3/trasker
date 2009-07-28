use strict;

use Test::More tests => 4;

BEGIN {
    use_ok('Trasker::TTDB::Projects');
};

my $projects = Trasker::TTDB::Projects->get();

my $count;

for my $project ($projects->entries()) {
    $count++;
}

is($count, 3, 'count');

ok($projects->get_entry(1), 'get_entry');

is($projects->get_entry(2)->longname, 'First:1.1', 'longname');

$projects->flush();


