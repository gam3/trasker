use strict;

use Test::More tests => 1;

BEGIN {
    use_ok('Trasker::TTDB::User');
};

our $user_A = Trasker::TTDB::User->get(user => 'bob');
our $user_B = Trasker::TTDB::User->get(user => 'bill');

$user_A->set_current_project(project_id => 1, host => 'none');
$user_B->set_current_project(project_id => 2, host => 'none');

sleep 1;

$user_A->set_current_project(project_id => 2, host => 'none');
$user_B->set_current_project(project_id => 1, host => 'none');

sleep 1;

$user_A->set_current_project(project_id => 3, host => 'none');
$user_B->set_current_project(project_id => 3, host => 'none');
