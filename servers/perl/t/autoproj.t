use strict;

use Tasker::TTDB::User;
use Tasker::TTDB::Project;
use Tasker::TTDB::Auto;

use Test::More tests => 1;

our $proj = Tasker::TTDB::Project->get(id => 2);
our $user = Tasker::TTDB::User->get(id => 1);
our $auto = Tasker::TTDB::Auto->get(
    user => $user,
    class => 'testme', 
    desktop => 'testme', 
    host => 'testme', 
    name => 'testme',
    role => 'testme',
    title => 'testme',
);

pass();

use Data::Dumper;

sleep 1;
$user->set_current_project(project_id => 1,
			   temporary => 'other',
                           host => 'test');
sleep 1;
$user->set_current_project(project_id => 2,
			   temporary => 'other',
                           host => 'test');
sleep 1;
$user->revert(type => 'other');
sleep 1;
$user->set_current_project(project_id => 2,
                           host => 'test');
