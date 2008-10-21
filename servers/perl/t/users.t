use strict;

use Test::More tests => 2;

BEGIN {
    use_ok('Tasker::TTDB::Users');
};

our $users = Tasker::TTDB::Users->new();

is($users->entries(), 2, 'two users');
