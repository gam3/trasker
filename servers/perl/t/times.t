use strict;

use Test::More tests => 2;
use Tasker::TTDB::User;
use Tasker::Date;

BEGIN {
    use_ok('Tasker::TTDB::Time');
};

our $user_A = Tasker::TTDB::User->get(user => 'bob');

my $time = $user_A->day(date => Tasker::Date->today());

ok(1);
