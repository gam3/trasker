use strict;

use Test::More tests => 2;
use Trasker::TTDB::User;
use Trasker::Date;

BEGIN {
    use_ok('Trasker::TTDB::Time');
};

our $user_A = Trasker::TTDB::User->get(user => 'bob');

my $time = $user_A->day(date => Trasker::Date->today());

ok(1);
