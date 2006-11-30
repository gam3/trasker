use strict;

use Test::More tests => 1;

use TTDB::DBI qw (get_dbh);

our $dbh = get_dbh;

pass('it works');

$dbh->do(qq/delete from auto/) or die;

$dbh->do(qq/SELECT setval('auto_id_seq', 1, false)/);

$dbh->do(qq/delete from notes/) or die;

$dbh->do(qq/SELECT setval('notes_id_seq', 1, false)/);

$dbh->do(qq/delete from timeslice/) or die;

$dbh->do(qq/SELECT setval('timeslice_id_seq', 1, false)/);

$dbh->do(qq/delete from user_project/) or die;

$dbh->do(qq/delete from project/) or die;

$dbh->do(qq/SELECT setval('project_id_seq', 1, false)/);

$dbh->do(qq/delete from users/) or die;

$dbh->do(qq/SELECT setval('users_id_seq', 1, false)/);

$dbh->commit;

