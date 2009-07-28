use strict;

use Test::More tests => 2;

BEGIN {
    use_ok('Trasker::TTDB::Users');
};

our $users = Trasker::TTDB::Users->new();

is($users->entries(), 2, 'two users');
