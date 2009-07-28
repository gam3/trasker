## @file
## 
##
#

## @class
# The Trasker Time DataBase package
package Trasker::TTDB;

use Trasker::TTDB::DBI qw (get_dbh);

sub now
{
    my $dbh = get_dbh();
    my @row_ary = $dbh->selectrow_array(<<SQL);
select now()
SQL
    return $row_ary[0];
}

1;
