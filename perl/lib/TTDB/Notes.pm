use strict;

package TTDB::Notes;

use Params::Validate;

use TTDB::DBI qw (get_dbh);

sub new
{
    my $class = shift;

    my %p = validate(@_, {
        date => {
	    optional => 1,
	    isa => 'Date::Calc::MySQL',
	},
	today => 0,
    });

    bless { %p }, $class;
}

sub entries
{
    my $self = shift;
    my $dbi = get_dbh;
    my @ret;
    my @args = ();

    my $sql = "select * from notes where 1 = 1";

    if ($self->{today}) {
        $sql .= ' and date(time) = date(now())';
    }
    if ($self->{date}) {
        $sql .= ' and date(time) = date(?)';
	push(@args, $self->{date}->mysql);
    }

    $sql .= ' order by time';

    my $sth = $dbi->prepare($sql);

    $sth->execute(@args);
    require TTDB::Note;

    map({bless $_, 'TTDB::Note'} @{$sth->fetchall_arrayref({})});
}

1;
__END__
