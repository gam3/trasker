use strict;

use Test::More tests => 3;

BEGIN {
    use_ok('TTDB::Auto');
    use_ok('TTDB::User');
};

eval {
    TTDB::Auto->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

my $auto = TTDB::Auto->new(user_id => 1, project_id => 1, role => 'testme');

$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1, name => 'testme');
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1, class => 'testme');
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1, title => 'testme');
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1, desktop => 'testme');
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
);
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme'
);
$auto->create();

$auto = TTDB::Auto->new(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme',
host => 'testme'
);
$auto->create();

$auto = TTDB::Auto->get(user_id => 1, role => 'testme', desktop => 'testme');

use Data::Dumper;
warn Dumper $auto;

$auto = TTDB::Auto->get(user_id => 1, name => 'testme', desktop => 'testme');

warn Dumper $auto;

$auto->delete();
