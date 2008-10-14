use strict;

use Test::More tests => 2;

BEGIN {
    use_ok('TTDB::Users');
};

our $users = TTDB::Users->new();

is($users->entries(), 2, 'two users');
