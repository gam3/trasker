use strict;

use Test::More tests => 9;

BEGIN {
    use_ok('TTDB::User');
};

eval {
    TTDB::User->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}
our $u1 = TTDB::User->new(name => 'bob', fullname => 'Bob Johnson');

is($u1->name, 'bob');

our $user_A = TTDB::User->get(user => 'gam3');
our $user_B = TTDB::User->get(id => $user_A->id());

is( $user_A->id, $user_B->id, 'compare ids');
is($user_A->id, 1, 'test id');
is( $user_A->name, $user_B->name, 'compare names' );
is($user_A->name, 'gam3', 'test names');
is( $user_A->fullname, $user_B->fullname, 'compare fullname');
is( $user_A->fullname, 'G. Allen Morris III', 'test fullname');


$user_A->add_note(note => "This is a user note");

