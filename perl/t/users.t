use strict;

use Test::More tests => 1;

BEGIN {
    use_ok('TTDB::Users');
};

our $users = TTDB::Users->new();

use Data::Dumper;
diag(Dumper $users);
