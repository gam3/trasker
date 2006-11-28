use strict;

package TTDB::Note;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw (validate);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        note => 1,
        type => 0,
        time => {
	    optional => 1,
	    isa => "Date::Calc",
	},
        user => {
	    optional => 1,
	    isa => "TTDB::User",
	},
        project => {
	    optional => 1,
	    isa => "TTDB::Project",
	},
    });

    bless { type => 1, %p }, $class;
}

sub time
{
    my $self = shift;

    $self->{time};
}

sub type
{
    my $self = shift;

    $self->{type};
}

sub user_id
{
    my $self = shift;

    if (my $u = $self->{user}) {
        return $u->id;
    } else {
        return 0;
    }
}

sub project_id
{
    my $self = shift;

    if (my $p = $self->{project}) {
        return $p->id;
    } else {
        return 0;
    }
}

sub create
{
    my $self = shift;
    my $dbh = get_dbh;
    my %p = validate(@_, {
    });

    my $note = $self->{note};
    my $time = $self->time;

    my $st;

    if ($time) {
	$st = $dbh->prepare("insert into notes (type, time, user_id, project_id, note) values (?, ?, ?, ?, ?)");

	$st->execute($self->type, $time->mysql, $self->user_id, $self->project_id, $note) or die $dbh->err_str();
    } else {
	$st = $dbh->prepare("insert into notes (type, time, user_id, project_id, note) values (?, now(), ?, ?, ?)");

	$st->execute($self->type, $self->user_id, $self->project_id, $note) or die $dbh->err_str();
    }

    $dbh->commit or die $dbh->err_str();

    $self;
}

sub get
{
    my $class = shift;
    my $dbh = get_dbh('read');
    my %p = validate(@_, {
        id => 1,
    });
    my @ret;
    my @args = ();

    my $sql = "select * from notes";

    $sql .= ' where id = ?';

    my $sth = $dbh->prepare($sql);

    $sth->execute($p{id});

    map({bless $_, 'TTDB::Note'} @{$sth->fetchall_arrayref({})});

    bless { @_ }, $class;
}

sub user
{
    my $self = shift;

    $self->{user_id};
}

sub project
{
    my $self = shift;

    $self->{project_id};
}

sub text
{
    my $self = shift;

    $self->{note};
}

1;
__END__
