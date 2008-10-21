use strict;

use Test::More tests => 5;

BEGIN {
    use_ok('Tasker::TTDB::User');
};

eval {
    Tasker::TTDB::User->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $u1 = Tasker::TTDB::User->new(name => 'bob', fullname => 'Bob Johnson');

is($u1->name, 'bob');

$u1->create();

is($u1->id, '1');

$u1 = Tasker::TTDB::User->new(name => 'bill', fullname => 'Bill Johnson');

$u1->create();

is($u1->id, '2');

