use strict;

use Test::More tests => 2;

use Tasker::TTDB::DBI qw (get_dbh);

use_ok('Tasker::TTDB::Auto');

our $dbh = get_dbh;

$dbh->do(qq/delete from auto/) or die;

$dbh->do(qq/SELECT setval('auto_id_seq', 1, false)/);

$dbh->commit;

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, host => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, name => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, class => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, role => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, title => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, desktop => 'testme');

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
);

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme'
);

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme',
host => 'testme'
);

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => '% testme %',
);

my $a = Tasker::TTDB::Auto->get(user_id => 1,
    host => 'testme',
    name => 'testme',
    class => 'testme',
    role => 'testme',
    desktop => 'testme',
    title => 'testme',
);

is($a->id, 9);

Tasker::TTDB::Auto->create(user_id => 1, project_id => 1, host => '%.test.net');

