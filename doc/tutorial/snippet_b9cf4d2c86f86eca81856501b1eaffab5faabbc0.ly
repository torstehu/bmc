% Automatically generated by BMC, the braille music compiler
\version "2.14.2"
\include "articulate.ly"
\header {
  tagline = ""
}
music =
  <<
    \new Staff {
      e''2 d''4 c'' | % 1
      a'4 d''2 c''4 | % 2
      b'4 e''2 d''4 | % 3
      c''2 f''4 e'' | % 4
      d''4 g''2 f''4 | % 5
      e''4 c'' a'' c'' | % 6
      b'8 c'' d'' e'' f'' g'' a'' b'' | % 7
      c'''2 r | % 8
    }
  >>

\score {
  \music
  \layout { }
}
\score {
  \unfoldRepeats \articulate \music
  \midi { }
}
