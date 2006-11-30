use strict;

use Test::More tests => 2;

BEGIN {
    use_ok('TTDB::Project');
};

eval {
    TTDB::Project->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $p1 = TTDB::Project->new(name => 'First', description => 'This is the first project');

$p1->create;

our $p2 = TTDB::Project->new(name => '1.1', description => 'This is the first sub project', parent => $p1);

$p2->create;

$p1 = TTDB::Project->new(name => 'Second', description => 'This is the second project');

$p1->create;

