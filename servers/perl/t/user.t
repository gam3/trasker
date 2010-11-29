use strict;

use Test::More tests => 5;

BEGIN {
    use_ok('Trasker::TTDB::User');
};

eval {
    Trasker::TTDB::User->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $user_A = Trasker::TTDB::User->new(id => 1, name => 'bob', fullname => "Test Me");

is($user_A->id, 1, 'test id');
is( $user_A->name, "bob", 'test name' );
is( $user_A->fullname, "Test Me", 'test fullname' );

#done_testing();
