
use strict;

package TTDB::Entry;
use TTDB::DBI qw (get_dbh);

sub longname
{
   my $self = shift;

   $self->{long_name};
}

sub name
{
   my $self = shift;

   $self->{project_name};
}

sub id
{
   my $self = shift;
   $self->{project_id};
}

sub add_child
{
   my $self = shift;
   my $child_id = shift;
   push(@{$self->{child}}, $child_id);

   $self;
}

sub child
{
   my $self = shift;
   $self->{child};
}

sub get_time
{
    my $self = shift;
    my $id = $self->id();
    my $dbh = get_dbh();

    my $sth = $dbh->prepare(<<SQL);
select SEC_TO_TIME(SUM(TIME_TO_SEC(ifnull(elapsed, now() - start_time))))
  from timeslice 
 where start_time >= date(now())
   and project_id = ?
 group by project_id;
SQL
    $sth->execute($id);
    my $time = $sth->fetchrow_array;

    $time;
}

sub time
{
   '2:22';
}

sub alltime
{
   '1:11';
}

1;
