#!/usr/bin/perl
use strict;

use Getopt::Long;
use Pod::Usage;

our $VERSION = "0.0.1";

use TTDB::UserProject;
use TTDB::User;
use TTDB::Project;
use TTDB::Notes;
use TTDB::Time;

use Date::Calc::Object;
use Date::Calc::MySQL;

use Data::Dumper;

use TTDB::DBI qw (dbi_setup);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

our ($opt_help, $opt_man, $opt_versions, $opt_user, $opt_project);
our $site = 'http://www.gam3.com/tasker';
our $opt_debug = 0;
our @opt_dates;

dbi_setup(
    host => 'localhost',
    user => 'gam3',
    database => 'tasker',
);

{
package Date::Calc::MySQL;
sub as_iso_week
{
    my $date = shift;
    my ($week, $year) = Week_of_Year($date->date), "\n";
    my $dow = Day_of_Week($date->date) - 1;
    $date = $date - $dow;
   
    sprintf("%04d-W%02d-%d\n", $year, $week, $dow);
}
sub bow
{
    my $date = shift;
    my ($week, $year) = Date::Calc::Week_of_Year($date->date), "\n";
    my $dow = Date::Calc::Day_of_Week($date->date) - 1;
    $date = $date - $dow;
warn $date, "\n";
    Date::Calc::Object->new(($date->date)[0..2]);
}
}

GetOptions(
  'date=s'    => \@opt_dates,
  'debug=i'   => \$opt_debug,
  'help!'     => \$opt_help,
  'man!'      => \$opt_man,
  'versions!' => \$opt_versions,
  'user=s'    => \$opt_user,
  'project=s' => \$opt_project,
) or pod2usage(-verbose => 1) && exit;

pod2usage(-verbose => 1) && exit if ($opt_debug !~ /^[01]$/);
pod2usage(-verbose => 1) && exit if defined $opt_help;
pod2usage(-verbose => 2) && exit if defined $opt_man;
exit if $opt_versions;

sub now {
    my @dates;
    my $date = Date::Calc->now();
    my $dow;
    $date -= 7;
    my ($week, $year) = Week_of_Year($date->date);
    $dow = Day_of_Week($date->date) - 1;
    $date = $date - $dow;
    printf("%04d-W%02d-%d\n", $year, $week, $dow);
    push @dates, Date::Calc::Object->new(($date->date)[0..2]);
}

if (0) {
    my $project = TTDB::Project->get(name => 'Peter');
    my $user = TTDB::User->get(user => 'gam3');

    for (my $date = Date::Calc::MySQL->new(2007, 1, 22); $date < Date::Calc::MySQL->now; $date += 7) {
	my $up = TTDB::UserProject->new(user => $user, project => $project);
	my ($week, $year) = Date::Calc::Week_of_Year($date->date);
	my $total = $up->week(date => $date, all => 1);
	print $date, " ", $date->strftime('%W'), ' ', $total->duration->as_hours, "\n";

    }
    exit;
}

sub main
{
    my %p = validate(@_, {
        project_name => 1,
        user_name => 1,
        date => {
	    isa => 'Date::Calc::MySQL',
	},
    });
    my $date = $p{date};
    my $project = TTDB::Project->get(name => $p{project_name});
    my $user = TTDB::User->get(user => $p{user_name});
    my $rate = 25;
    my ($week, $year) = Date::Calc::Week_of_Year($date->date);

    my $bow = $date->bow;

    my $sproj = $p{project_name};
    my $filename = sprintf("%s-%s-w%02d-%s.tex", $sproj, $year, $week, $p{user_name});

    my $up = TTDB::UserProject->new(user => $user, project => $project);

    my $x = Date::Calc::Date_to_Text_Long(Date::Calc->today->date);
    my $y = Date::Calc::Date_to_Text_Long($date->date);
    my ($week, $year) = Date::Calc::Week_of_Year($date->date);

    my $total = $up->week(date => $date, all => 1);

    $x =~ s/^.*,//;
    $y =~ s/^.*,//;

    my $data_1;
    {
	my $name  = $up->project->name;
	my $rate  = $up->rate;
	my $hours_d = $up->week(date => $date);
	next unless $hours_d->duration;
	my $hours = $hours_d->duration->as_hours;
	my $amount = $rate * $hours;
	$name = "Misc.";

	if ($amount) {
	    $data_1 .= q(\\FSdesc{) . $name .
		       qq(}{) . $rate . qq(}{) .
		       sprintf("%0.1f", $hours) . qq(}{) .
		       sprintf('%0.2f', $amount) . qq(}\n);
	}
    }
    for my $cup ($up->children) {
	my $name  = $cup->project->name;
	my $rate  = $cup->rate;
	my $hours_d = $cup->week(date => $date);
	my $hours = $hours_d->duration->as_hours;
	my $amount = $rate * $hours;

	next unless $amount;

        $data_1 .= q(\\FSdesc{) . $name .
	           qq(}{) . $rate . qq(}{) .
		   sprintf("%0.1f", $hours) . qq(}{) .
		   sprintf('%0.2f', $amount) . qq(}\n);
    }
    my $data_2 = '';
    my $sub_total = 0;

    for (my $d = $date; $d < $date + 7; $d++) {
	my $dstr = Date::Calc::Date_to_Text($d->date);
	my $hours_d = $up->day(date => $d, all => 1);
	my $h = $hours_d->duration->as_hours;
	$sub_total += $h;
        $data_2 .= <<X;
$dstr & Misc & $h \\\\
X
	for my $notes ($hours_d->get_notes(exclude_type => 99)) {
	    my $text = $notes->text;

	    $text =~ s/\\n/ - /g;
	    $data_2 .= <<X;
& \\parbox[l]{8.2cm}{ $text } & \\\\
X
	}
    }

    open OUT, ">$filename" or die;

    print OUT invoice(
        date      => $x,
        date_text => $y,
        invnum    => sprintf('%s %05d', $sproj, $week),
        email     => q("G. Allen Morris III" $<$gam3@gam3.com$>$),
        co_name   => q(Almond Hill Enterprises),
        co_add1   => q(125 Chaparral Court),
        co_add2   => q(Suite 100),
        co_add3   => q(Anaheim Hills, CA 92808-2263),
        total     => sprintf("%0.2f", $total->duration->as_hours * $rate ),
        data_1    => $data_1,

        data_2    => $data_2,
    );
    close OUT or die;
}

END {
    if (defined $opt_versions) {
	print
	  "\nModules, Perl, OS, Program info:\n",
	  "  TTDB::User            $TTDB::User::VERSION\n",
	  "  Pod::Usage            $Pod::Usage::VERSION\n",
	  "  Getopt::Long          $Getopt::Long::VERSION\n",
	  "  strict                $strict::VERSION\n",
	  "  Perl                  $]\n",
	  "  OS                    $^O\n",
	  "  $0\t\t$VERSION\n",
	  "  $site\n",
	  "\n\n";
    }
}

sub invoice
{
    my %data = validate(@_, {
        date      => 1,
        date_text => 1,
        invnum    => 1,
        email     => 1,
        co_name   => 1,
        co_add1   => 1,
        co_add2   => 0,
        co_add3   => 0,
        co_add4   => 0,
        total     => 1,
        data_1    => 1,
        data_2    => 1,
    });

my $tex = <<'TEX';
\documentclass[letterpaper]{article}
\usepackage{fancyhdr,lastpage,ifthen,longtable,afterpage}
\usepackage{graphicx}			% required for logo graphic

\renewcommand{\date}{@@date@@}
\newcommand{\invnum}{@@invnum@@}
\newcommand{\poline}{~}
\newcommand{\notes}{Notes:}

\newcommand{\footer}{@@email@@}
\newcommand{\smallfooter}{@@email@@}

\addtolength{\voffset}{-0.0cm}		% top margin to top of header
\addtolength{\hoffset}{-0.6cm}		% left margin on page
\addtolength{\topmargin}{-1.25cm}	% top margin to top of header
\setlength{\headheight}{2.0cm} 		% height of header
\setlength{\headsep}{1.0cm}		% between header and text
\setlength{\footskip}{1.0cm}		% bottom of footer from bottom of text

%\addtolength{\textwidth}{2.1in}    	% width of text
\setlength{\textwidth}{19.5cm}
\setlength{\textheight}{19.5cm}
\setlength{\oddsidemargin}{-0.9cm} 	% odd page left margin
\setlength{\evensidemargin}{-0.9cm} 	% even page left margin

\renewcommand{\headrulewidth}{0pt}
\renewcommand{\footrulewidth}{1pt}

% Adjust the inset of the mailing address
\newcommand{\addressinset}[1][]{\hspace{1.0cm}}

% Adjust the inset of the return address and logo
\newcommand{\returninset}[1][]{\hspace{-0.25cm}}

% New command for address lines i.e. skip them if blank
\newcommand{\addressline}[1]{\ifthenelse{\equal{#1}{}}{}{#1\newline}}

% Inserts dollar symbol
\newcommand{\dollar}[1][]{\symbol{36}}

% Remove plain style header/footer
\fancypagestyle{plain}{
  \fancyhead{}
}
\fancyhf{}

% Define fancy header/footer for first and subsequent pages
\fancyfoot[C]{
  \ifthenelse{\equal{\thepage}{1}}
  { % First page
    \small{
\footer
    }
  }
  { % ... pages
    \small{
\smallfooter
    }
  }
}

\fancyfoot[R]{
  \ifthenelse{\equal{\thepage}{1}}
  { % First page
  }
  { % ... pages
    \small{\thepage\ of \pageref{LastPage}}
  }
}

\fancyhead[L]{
  \ifthenelse{\equal{\thepage}{1}}
  { % First page
    \returninset
    \makebox{
      \begin{tabular}{ll}
        \includegraphics{logo} &
        \begin{minipage}[b]{5.5cm}
G. Allen Morris III\\
139 Olive Street\\
Saranac Lake, New York 12983
        \end{minipage}
      \end{tabular}
    }
  }
  { % ... pages
    %\includegraphics{logo}	% Uncomment if you want the logo on all pages.
  }
}

\fancyhead[R]{
  \ifthenelse{\equal{\thepage}{1}}
  { % First page
    \begin{tabular}{rcl}
    Invoice date & & Invoice number \\
    \vspace{0.2cm}
    \textbf{\date} & & \textbf{\invnum} \\\hline
    \rule{0pt}{3ex}
    \textsc{Amount Due:} & \textsc{
\$@@total@@
} & \\
    \textsc{Amount Enclosed:} & \textsc{............} & \\
    \vspace{-0.2cm}
     & & \\\hline
    \end{tabular}
  }
  { % ... pages
    \small{
      \begin{tabular}{ll}
      Invoice date & Invoice number\\
      \textbf{@@date@@} & \textbf{@@invnum@@}\\
      \end{tabular}
    }
  }
}

\pagestyle{fancy}

%% Font options are:
%%	bch	Bitsream Charter
%% 	put	Utopia
%%	phv	Adobe Helvetica
%%	pnc	New Century Schoolbook
%%	ptm	Times
%%	pcr	Courier

\renewcommand{\familydefault}{phv}

% Commands for freeside description...
\newcommand{\FSdesc}[4]{
  {#1} &
  {\dollar #2} &
  {#3} &
  {\dollar #4} \\
}
% ...extended description...
\newcommand{\FSextdesc}[1]{
  \multicolumn{1}{l}{\rule{0pt}{1.0ex}} &
  \multicolumn{2}{l}{\small{~-~#1}}\\
}
% ...and total line items.
\newcommand{\FStotaldesc}[2]{
  & \multicolumn{2}{r}{#1} & \textbf{\dollar #2}\\
}

\begin{document}
\begin{tabular}{ll}
\addressinset \rule{0cm}{0cm} &
\makebox{
\begin{minipage}[t]{5.0cm}
\vspace{0.25cm}
\textbf{@@co_name@@}\\
\addressline{@@co_add1@@}
\addressline{@@co_add2@@}
\addressline{@@co_add3@@}
\addressline{@@co_add4@@}
\rule{0pt}{8.5ex}

\end{minipage}}
\end{tabular}
\hfill

\vspace{0.5cm}
%
\section*{\textsc{Charges for the week starting @@date_text@@}}

\begin{longtable}{lrrr}
\rule{0pt}{2.5ex}
\makebox[8.4cm][l]{\textbf{Description}} &
\makebox[1.8cm][r]{\textbf{Rate}} &
\makebox[1.8cm][r]{\textbf{Hours}} &
\makebox[1.8cm][r]{\textbf{Amount}} \\
\hline
\endfirsthead
\multicolumn{4}{r}{\rule{0pt}{2.5ex}Continued from previous page}\\
\hline
\rule{0pt}{2.5ex}
\makebox[8.4cm][l]{\textbf{Description}} &
\makebox[1.0cm][r]{\textbf{Rate}} &
\makebox[0.7cm][r]{\textbf{Hours}} &
\makebox[1.8cm][r]{\textbf{Amount}} \\
\hline
\endhead
\multicolumn{4}{r}{\rule{0pt}{2.5ex}Continued on next page...}\\
\endfoot
\hline
\FStotaldesc{Totals for the week}{@@total@@}\\
\endlastfoot

\hline
@@data_1@@\hline

\end{longtable}

\begin{longtable}{clr}
\makebox[1.4cm][l]{\textbf{Date}} &
\makebox[8.4cm][l]{\textbf{Description}} &
\makebox[1.5cm][r]{\textbf{Hours}} \\
\hline
@@data_2@@
\hline
\end{longtable}

\vfill
\notes
\end{document}
TEX

    $tex =~ s/@@([^@]*)@@/$data{$1} or ''/eg;

    $tex;
}

die 'need a project' unless $opt_project;
die 'need a user' unless $opt_user;
die 'need a date' unless @opt_dates;

for my $tdate (@opt_dates) {
    my $date;
    if ($tdate =~ /w/) {
        my ($year, $week) = split('-', $tdate);
	$week =~ s/w//;
	$date = Date::Calc::MySQL->new(Date::Calc::Monday_of_Week($week, $year));
    } else {
	$date = Date::Calc::MySQL->new(split('-', $tdate));
    }
    
    main(
	project_name => $opt_project,
	user_name => $opt_user,
	date => $date,
    );
}

__END__

=head1 NAME

 invoice

=head1 SYNOPSIS

  invoice --project project [date [date ...]]

=head1 DESCRIPTION

  generate an invoice for a week

=head1 ARGUMENTS

  arguments are a list of projects.

  an invoice will be created for each date

=head1 OPTIONS

=over 5

=item B<--user> I<userid>

Control tasker for this tasker userid.

=item B<--project> I<project name>

=item B<--versions>

print program version

=back

=head1 AUTHOR

G. Allen Morris III <tasker@gam3.net>

=head1 BUGS

None that I know of.

=head1 TODO

We could use more security.

=head1 UPDATES

 2006-10-08   06:22 EST
   Initial working code

=cut

    for (@opt_dates) {
	if (my $time = str2time($_)) {
	    my $date = Date::Calc::Object->time2date($time);
	    print $date->as_iso_week;
	    push @dates, $date->bow;
	} elsif (my ($y, $ws, $we) = (/(\d{4})-[Ww](\d+)-[Ww](\d+)$/)) {
	    my $date = Date::Calc::Object->new(Monday_of_Week($ws, $y));
	    my $c = $we - $ws + 1;
	    while ($c--) {
		push @dates, $date;
		$date += 7;
	    }
	} elsif (my ($y, $ws, $c) = (/(\d{4})-[Ww](\d+)-(\d+)$/)) {
	    my $date = Date::Calc::Object->new(Monday_of_Week($ws, $y));
	    while ($c--) {
		push @dates, $date;
		$date += 7;
	    }
	} elsif (my ($y, $w) = (/(\d{4})-[Ww](\d+)$/)) {
	    my $date = Date::Calc::Object->new(Monday_of_Week($w, $y));
	    push @dates, $date;
	} elsif (my ($w) = (/[Ww](\d+)$/)) {
	    my $y = 2007;
	    my $date = Date::Calc::Object->new(Monday_of_Week($w, $y));
	    push @dates, $date;
	} else {
	    die "Did not recognize $_";
	}
    }

