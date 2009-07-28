use strict;

use Test::More tests => 2;

BEGIN {
    use_ok('Trasker::TTDB::Project');
};

use Trasker::TTDB::DBI qw (get_dbh);

our $dbh = get_dbh;

$dbh->do('delete from notes');
$dbh->do(qq/SELECT setval('notes_id_seq', 1, false)/);
$dbh->do('delete from timeslice');
$dbh->do(qq/SELECT setval('timeslice_id_seq', 1, false)/);
$dbh->do('delete from project');
$dbh->do(qq/SELECT setval('project_id_seq', 1, false)/);
$dbh->do(qq/delete from user_project/) or die;

$dbh->commit;

eval {
    Trasker::TTDB::Project->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $p1 = Trasker::TTDB::Project->new(name => 'First', description => 'This is the first project');

$p1->create;

our $p2 = Trasker::TTDB::Project->new(name => '1.1', description => 'This is the first sub project', parent => $p1);

$p2->create;

$p1 = Trasker::TTDB::Project->new(name => 'Second', description => 'This is the second project');

$p1->create;

