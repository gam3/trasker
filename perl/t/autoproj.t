use strict;

use TTDB::User;
use TTDB::Project;
use TTDB::Auto;

use Test::More tests => 1;

our $proj = TTDB::Project->get(id => 2);
our $user = TTDB::User->get(id => 1);
our $auto = TTDB::Auto->get(
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
