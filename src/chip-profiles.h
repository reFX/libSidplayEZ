//
// This file contains chip-profiles which allow us to adjust
// certain settings that varied wildly between 6581 chips, even
// made in the same factory on the same day.
//
// This works under the assumption that the authors used the
// same SID chip their entire career.
//
{ "Anthony Lees",						{	.folder = "/MUSICIANS/L/Lees_Anthony/",				.filter = 1.3	} },
{ "Antony Crowther (Ratt)",				{	.folder = "/MUSICIANS/C/Crowther_Antony/",			.filter = 1.1,	} },
{ "Ben Daglish",						{	.folder = "/MUSICIANS/D/Daglish_Ben/",				.filter = 0.6,	.exceptions = { { "Last_Ninja", "Anthony Lees" } } } },	// Only Anthony Lees used the filter
{ "Charles Deenen",						{	.folder = "/MUSICIANS/D/Deenen_Charles/",			.filter = 0.2	} },
{ "Chris Hülsbeck",						{	.folder = "/MUSICIANS/H/Huelsbeck_Chris/",			.filter = 0.6,	.digi = 0.95	} },
{ "Clever Music",						{	.folder = "/MUSICIANS/C/Clever_Music/",				.filter = 0.25	} },
{ "Mitch & Dane",						{	.folder = "/MUSICIANS/M/Mitch_and_Dane/",			.filter = 0.75	} },
{ "Michael Nilsson-Vonderburgh (Mitch)",{	.folder = "/MUSICIANS/M/Mitch_and_Dane/Mitch/",		.filter = 0.25	} },
{ "Stellan Andersson (Dane)",			{	.folder = "/MUSICIANS/M/Mitch_and_Dane/Dane/",		.filter = 0.75	} },
{ "David Dunn",							{	.folder = "/MUSICIANS/D/Dunn_David/",				.filter = 0.15	} },
{ "David Whittaker",					{	.folder = "/MUSICIANS/W/Whittaker_David/",			.filter = 0.15	} },
{ "Thomas Mogensen (DRAX)",				{	.folder = "/MUSICIANS/D/DRAX/",						.filter = 0.3	} },
{ "20th Century Composers",				{	.folder = "/MUSICIANS/0-9/20CC/",					.filter = 0.4	} },
{ "Edwin van Santen",					{	.folder = "/MUSICIANS/0-9/20CC/van_Santen_Edwin/",	.filter = 0.5	} },
{ "Falco Paul",							{	.folder = "/MUSICIANS/0-9/20CC/Paul_Falco/",		.filter = 0.15	} },
{ "Figge Wasberger (Fegolhuzz)",		{	.folder = "/MUSICIANS/F/Fegolhuzz/",				.filter = 0.25,	.zeroDac = 0.5	} },
{ "Fred Gray",							{	.folder = "/MUSICIANS/G/Gray_Fred/",				.filter = 0.18,	.zeroDac = 0.2	} },
{ "Kim Christensen (Future Freak)",		{	.folder = "/MUSICIANS/F/Future_Freak/",				.filter = 0.35	} },
{ "Geir Tjelta",						{	.folder = "/MUSICIANS/T/Tjelta_Geir/",				.filter = 0.5	} },
{ "Georg Feil",							{	.folder = "/MUSICIANS/F/Feil_Georg/",				.filter = 0.2,	.zeroDac = 0.2	} },
{ "Glenn Gallefoos",					{	.folder = "/MUSICIANS/B/Blues_Muz/Gallefoss_Glenn/",.filter = 0.2,	.zeroDac = 0.2	} },
{ "Jason Page",							{	.folder = "/MUSICIANS/P/Page_Jason/",				.filter = 0.35	} },
{ "Jeroen Tel",							{	.folder = "/MUSICIANS/T/Tel_Jeroen/",				.filter = 0.35,	.digi = 0.95	} },
{ "Johannes Bjerregaard",				{	.folder = "/MUSICIANS/B/Bjerregaard_Johannes/",		.filter = 0.35	} },
{ "Jonathan Dunn",						{	.folder = "/MUSICIANS/D/Dunn_Jonathan/",			.filter = 0.25,	.digi = 0.8	} },
{ "Jouni Ikonen (Mixer)",				{	.folder = "/MUSICIANS/M/Mixer/",					.filter = 0.25	} },
{ "Laxity",								{	.folder = "/MUSICIANS/L/Laxity/",					.filter = 0.3	} },
{ "Linus Åkesson (lft)",				{	.folder = "/MUSICIANS/L/Lft/",						.filter = 0.3	} },
{ "Mark Cooksey",						{	.folder = "/MUSICIANS/C/Cooksey_Mark/",				.filter = 0.25,	.zeroDac = 0.3	} },
{ "Markus Müller",						{	.folder = "/MUSICIANS/M/Mueller_Markus/",			.filter = 0.3	} },
{ "Martin Galway",						{	.folder = "/MUSICIANS/G/Galway_Martin/",			.filter = 1.0,	.zeroDac = 0.5	} },
{ "Martin Walker",						{	.folder = "/MUSICIANS/W/Walker_Martin/",			.filter = 0.15	} },
{ "Matt Gray",							{	.folder = "/MUSICIANS/G/Gray_Matt/",				.filter = 0.3	} },
{ "Michael Hendriks",					{	.folder = "/MUSICIANS/F/FAME/Hendriks_Michael/",	.filter = 0.25,	.digi = 1.5	} },
{ "Neil Brennan",						{	.folder = "/MUSICIANS/B/Brennan_Neil/",				.filter = 0.25,	.digi = 1.0	} },
{ "Peter Clarke",						{	.folder = "/MUSICIANS/C/Clarke_Peter/",				.filter = 0.2	} },
{ "Pex Tufvesson",						{	.folder = "/MUSICIANS/M/Mahoney/",					.filter = 0.35	} },
{ "Reyn Ouwehand",						{	.folder = "/MUSICIANS/O/Ouwehand_Reyn/",			.filter = 0.25,	.digi = 1.15	} },
{ "Richard Joseph",						{	.folder = "/MUSICIANS/J/Joseph_Richard/",			.filter = 0.3	} },
{ "Rob Hubbard",						{	.folder = "/MUSICIANS/H/Hubbard_Rob/",				.filter = 0.35,	.exceptions = { { "BMX_Kidz", "Jori Olkkonen (Yip)"	} } } },		// Only Yip used the filter for subtune 4
{ "Russell Lieblich",					{	.folder = "/MUSICIANS/L/Lieblich_Russell/",			.filter = 0.25,	.digi = 0.8,	.zeroDac = 0.25	} },
{ "Steve Turner",						{	.folder = "/MUSICIANS/T/Turner_Steve/",				.filter = 0.6,	.exceptions = { { "Bushido", "Jason Page"	} } } },	// Only Jason Page used the filter for subtune 2
{ "Tim Follin",							{	.folder = "/MUSICIANS/F/Follin_Tim/",				.filter = 0.5,	.zeroDac = 0.15	} },
{ "Jori Olkkonen (Yip)",				{	.folder = "/MUSICIANS/Y/Yip/",						.filter = 0.125	} },
{ "Zoci-Joe",							{	.folder = "/MUSICIANS/Z/Zoci-Joe/",					.filter = 0.3	} },
{ "Paul Hannay (Feekzoid)",				{	.folder = "/MUSICIANS/F/Feekzoid/",					.digi = 1.1 } },
{ "Barry Leitch (The Jackal)",			{	.folder = "/MUSICIANS/L/Leitch_Barry/",				.filter = 0.05,	.digi = 1.25 } },
{ "Markus Schneider",					{	.folder = "/MUSICIANS/S/Schneider_Markus/",			.digi = 1.1	} },
{ "Ramiro Vaca",						{	.folder = "/MUSICIANS/V/Vaca_Ramiro/",				.digi = 0.85	} },
