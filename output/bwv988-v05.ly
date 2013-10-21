% Automatically generated by BMC, the braille music compiler
\version "2.14.2"
\include "articulate.ly"
music =
  <<
    \new PianoStaff <<
      \new Staff = "RH" {
        \clef "treble"
        \key g \major
        \time 3/4
        r16 d'-1 e' fis' g'[ fis' g'-1 a'] b'[ a' g' b'] | % 1
        a'16[ g' fis'-3 e'] d'[ e' fis' g'] a'[ fis' e' d'] | % 2
        g'16[ fis' e' d'] cis'[ d'-1 e' fis'] g'-1[ b' a' g'] | % 3
        fis'16-3[ e' d' cis'-2] d'[ a d'-2 e'] fis'[ d'-1 fis' a'] | % 4
        d''16[ b' a' b'] g'[ b'-4 a' b'] d'[ g'-5 fis' g'] | % 5
        e'16[ g' d' g'] c'[ g' b g'] a[ g' b g'] | % 6
        c'16-2[ g' b g'] c'-1[ fis'-4 e' fis'] d'[ fis' c' fis'] | % 7
        b16[ g' fis' e'] d' c' b-3 a g8 r | % 8
        b''8-5 r g'-2 r b''4 | % 9
        a''8 r fis'-2 r a''4-4 | % 10
        g''16[ fis'' e'' dis''-2] e''[ g'' d''-1 g''] cis''[ g'' b' g''] | % 11
        cis''16 g'' e'' g'' a'8 g''-3 fis'' g''\turn  | % 12
        a''8 e'-2 fis' fis''-2 g'' a'' | % 13
        b''8 fis'-3 e' g''-3 a'' b'' | % 14
        e''8 g'-5 a e'-4 cis' a | % 15
        d8-2 d' fis'-2 a' d''4  \bar ":|:" % 16
         fis''8-4 r d'-2 r fis''4 | % 17
        g''8 d''16 g''-3 a''8 d''16 a'' b''8 g'' | % 18
        e''8-1 c''-3 a'-2 g'' fis''32 e'' dis''16 e''8~ | % 19
        e''8 dis'' r a-4 g fis | % 20
        e8 r e''-5 r d4 | % 21
        c8 r e'' r a,4 | % 22
        b,8 r g''-5 fis'' e'' dis'' | % 23
        e''8 r e4~-5 e16[ d c b,] | % 24
        c16-1[ e d e] a[ e d e-1] c'[ a-3 g a-1] | % 25
        fis'16[ a g-2 a-1] g'[ d'-3 c' d'-1] b'[ g' fis' g'-1] | % 26
        e''16[ d'' c''-1 d''] fis''[ d'' c'' d''] g''[ d'' c'' d''] | % 27
        a''16[ d'' c'' b'-3] c''[ b' a' g'] fis'[ a''-5 g'' fis''] | % 28
        g''8 b' r f-4 e d | % 29
        c16[ c' b-3 c'] d'[ c'-1 e' c'] fis'[ c' g' c'] | % 30
        a'16 c' b-3 a b8 d'' d-2 c''-5 | % 31
        b'16[ g' d' g'] b'[ g'-1 d'' b'] g''4  \bar ":|" % 32
      }
      \new Staff = "LH" {
        \clef "bass"
        \key g \major
        \time 3/4
        g8-5 r b'-2 r g4-3 | % 1
        fis8 r a'-2 r fis4-3 | % 2
        e8 r g'-2 r e4-4 | % 3
        d8 r fis'-2 r d-3 c | % 4
        b,8 r d'' r b,4-4 | % 5
        c8 r e'' r c4 | % 6
        d8 r a' r d4-2 | % 7
        g,8 r b'4~ b'16[ a' g' fis'] | % 8
        g'16[ b' cis''-3 d''] e''[ fis''-2 e''-1 d''] cis''[ b'-1 a' g'] | % 9
        fis'16[ a' b' cis''-3] d''[ e'' d'' cis''] b'-1[ a' g' fis'] | % 10
        e'8 fis' g' b' e''4~\mordent -2 | % 11
        e''8 d'' cis''16[ e'' b' e''-1] a'-4[ e'' g' e''] | % 12
        fis'16[ e'' d'' cis''] d''-1[ a' d' d''] e'-5[ d'' fis' d''] | % 13
        g'16-5[ e'' d'' cis''] d''-1[ b' e' d''] fis'[ d'' g'-5 d''] | % 14
        a'16[ d'' cis'' b'] cis''[ e'' a'-3 cis''] e'[ g'-2 fis' e'] | % 15
        fis'16[ a' d' fis'-2] a[ d'-1 fis a] d4  \bar ":|:" % 16
         r16 d'' c'' b' a'-1[ g' fis' e'] d'[ c'' b' a'] | % 17
        b'16-1[ g'-2 fis' e'] d'-1[ c' b a] g[ g' a'-3 b'] | % 18
        c''16[ d''-3 e'' d''] c''-1[ b' a' g'] a'[ b' c'' a'] | % 19
        b'16-1[ a' b' fis'-2] b[ cis' dis' e'-1] fis'-3[ g' a' fis'-4] | % 20
        g'16[ b' g' e'] g'[ b' g' e'] g'[ b' g' e'] | % 21
        a'16[ c'' a' e'] g'[ c'' g' e'] fis'[ c'' fis' e'] | % 22
        g'16[ b' g'-2 e'-3] b[ b' a' b'] g'[ b' fis' b'] | % 23
        g'16[ e' g'-4 b'] e'' b' g' b' e'8 r | % 24
        e'8 r c r e'4 | % 25
        d'8 r b, r d'4 | % 26
        c'8 b a-1 g fis e | % 27
        fis8 g16 a d4~ d16[ c' b a] | % 28
        b16-1[ g-3 fis g] a[ g b g-5] c'[ g d' g] | % 29
        e'8 g r b-2 a g | % 30
        fis16[ d'-2 e' fis'-2] g'[ d' b g] a[ c' g' fis'-3] | % 31
        g'8 g-1 d b, g,4  \bar ":|" % 32
      }
    >>
  >>

\score {
  \music
  \layout { }
}
\score {
  \unfoldRepeats \articulate \music
  \midi { }
}
