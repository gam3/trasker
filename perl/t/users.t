use strict;

use Test::More tests => 1;

BEGIN {
    use_ok('TTDB::Users');
};

our $proj = TTDB::Users->new();

use Data::Dumper;
#diag(Dumper $users);
