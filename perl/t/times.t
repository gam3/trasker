use strict;

use Test::More tests => 2;
use Date::Calc::MySQL;
use TTDB::User;

BEGIN {
    use_ok('TTDB::Time');
};

our $user_A = TTDB::User->get(user => 'bob');

my $time = $user_A->day(date => Date::Calc->today());

ok(1);
