// Copyright (C) 2012  Mario Lang <mlang@delysid.org>
//
// Distributed under the terms of the GNU General Public License version 3.
// (see accompanying file LICENSE.txt or copy at
//  http://www.gnu.org/licenses/gpl-3.0-standalone.html)

#include "bmc/lilypond.hpp"
#include <algorithm>
#include <iterator>

namespace music { namespace lilypond {

generator::generator( std::ostream &os
                    , bool layout
                    , bool midi
                    , bool include_locations)
: os(os)
, layout(layout)
, midi(midi)
, include_locations(include_locations)
, last_type(), last_dots(0)
{
  os << "% Automatically generated by BMC, the braille music compiler"
     << std::endl;
  os << "\\version" << " " << "\"2.14.2\"" << std::endl;
  os << "\\include" << ' ' << "\"articulate.ly\"" << std::endl;
}

void
generator::operator() (braille::ast::score const &score)
{
  if (no_tagline)
    os << "\\header {" << std::endl
       << "  tagline = \"\"" << std::endl
       << "}" << std::endl;
  os << "music =" << std::endl;
  os << "  " << "<<" << std::endl;
  for (auto const& part: score.unfolded_part) (*this)(part, score);
  os << "  " << ">>" << std::endl << std::endl;

  if (layout)
    os << "\\score {" << std::endl
       << "  " << "\\music" << std::endl
       << "  " << "\\layout { }" << std::endl
       << "}" << std::endl;
  if (midi)
    os << "\\score {" << std::endl
       << "  " << "\\unfoldRepeats \\articulate \\music" << std::endl
       << "  " << "\\midi { }" << std::endl
       << "}" << std::endl;
}

namespace {

struct repeat_info: public boost::static_visitor<void>
{
  bool begin, end;
  repeat_info(braille::ast::unfolded::staff_element const& staff_element)
  : begin(false)
  , end(false)
  { apply_visitor(*this, staff_element); }  

  result_type operator() (braille::ast::unfolded::measure const& measure)
  {
    for (auto const& voice: measure.voices)
      for (auto const& partial_measure: voice)
        for (auto const& partial_voice: partial_measure)
          for_each( partial_voice.begin(), partial_voice.end()
                  , apply_visitor(*this));
  }
  result_type operator() (braille::ast::key_and_time_signature const&) {}

  result_type operator() (braille::ast::barline const &barline)
  {
    switch (barline) {
    case braille::ast::begin_repeat: begin = true; break;
    case braille::ast::end_repeat: end = true; break;
    }
  }

  template<typename T>
  void operator() (T const &) const {}
};

}

void
generator::operator() ( braille::ast::unfolded::part const &part
                      , braille::ast::score const &score
                      )
{
  bool const keyboard = part.size() == 2;
  indent = "    ";
  if (keyboard) {
    os << indent << "\\new PianoStaff ";
    if (not default_instrument.empty())
      os << "\\with {midiInstrument = #\"" << default_instrument << "\"} ";
    os << "<<" << std::endl;
    indent += "  ";
  }
  for (size_t staff_index = 0; staff_index < part.size(); ++staff_index) {
    os << indent << "\\new Staff ";
    if (keyboard) {
      switch (staff_index) {
      case 0: os << "= \"RH\" "; break;
      case 1: os << "= \"LH\" "; break;
      }
    }
    if (not keyboard and not default_instrument.empty())
      os << "\\with {midiInstrument = #\"" << default_instrument << "\"} ";
    os << "{" << std::endl;

    if (keyboard) {
      switch (staff_index) {
      case 0: os << indent << "  "; ly_clef("treble"); break;
      case 1: os << indent << "  "; ly_clef("bass"); break;
      default: BOOST_ASSERT(false);
      }
      os << std::endl;
    }

    if (score.key_sig != 0) {
      os << indent << "  "; ly_key(score.key_sig); os << std::endl;
    }

    if (not score.time_sigs.empty())
      os << indent << "  " << "\\time" << " " << score.time_sigs.front() << std::endl;

    unsigned int measure_number = 1;
    if (not part[staff_index].empty()) {
      rational first_measure_duration(duration(part[staff_index].front()));
      if ((score.time_sigs.empty() and first_measure_duration != 1) or
          (not score.time_sigs.empty() and score.time_sigs.front() != first_measure_duration)) {
        os << indent << "  "; ly_partial(first_measure_duration); os << std::endl;
        measure_number = 0; // count from zero if we are dealing with upbeat
      }
    }

    for (std::size_t measure_index = 0;
         measure_index < part[staff_index].size();
         ++measure_index)
    {
      std::size_t new_measure_index = process_repeat_with_alternatives(part[staff_index], measure_index);

      while (new_measure_index != measure_index) {
        measure_index = new_measure_index;
        new_measure_index = process_repeat_with_alternatives(part[staff_index], measure_index);
      }

      if (measure_index < part[staff_index].size()) {
      braille::ast::unfolded::staff_element const&
      this_measure = part[staff_index][measure_index];

      os << indent << "  "; apply_visitor(*this, this_measure);

      bool barcheck = true;
      repeat_info this_repeat(this_measure);
      if (measure_index < part[staff_index].size() - 1) {
        braille::ast::unfolded::staff_element const&
        next_measure = part[staff_index][measure_index + 1];
        repeat_info next_repeat(next_measure);
        if (this_repeat.end and next_repeat.begin) {
          os << " " << "\\bar \":|:\"" << " ";
          barcheck = false;
        } else if (next_repeat.begin) {
          os << " " << "\\bar \"|:\"" << " ";
          barcheck = false;
        }
      }
      if (barcheck and this_repeat.end) {
        os << " " << "\\bar \":|\"" << " ";
        barcheck = false;
      }
      if (barcheck) os << " | ";
      os << "% " << measure_number++ << std::endl;
      }
    }
    os << indent << "}" << std::endl;
  }
  if (part.size() == 2) os << "    " << ">>" << std::endl;
}

generator::result_type
generator::operator() (braille::ast::key_and_time_signature const &key_and_time_sig)
{
  os << " \\time "
     << key_and_time_sig.time.numerator()
     << '/'
     << key_and_time_sig.time.denominator()
     << ' ';
}

generator::result_type
generator::operator() (braille::ast::unfolded::measure const &measure)
{
  if (measure.count > 1) os << "\\repeat unfold " << measure.count << " { ";
  if (measure.voices.size() == 1) {
    (*this)(measure.voices.front());
  } else {
    os << "<< ";
    for (size_t voice_index = 0; voice_index < measure.voices.size();
         ++voice_index)
    {
      os << "{"; (*this)(measure.voices[voice_index]); os << "}";
      if (voice_index != measure.voices.size() - 1) os << "\\\\";
    }
    os << " >>";
  }
  if (measure.count > 1) os << " }";
}

void
generator::operator() (braille::ast::unfolded::voice const &voice)
{
  for (size_t partial_measure_index = 0; partial_measure_index < voice.size();
       ++partial_measure_index)
  {
    (*this)(voice[partial_measure_index]);
    if (partial_measure_index != voice.size() - 1) os << " ";
  }
}

void
generator::operator() (braille::ast::unfolded::partial_measure const &partial_measure)
{
  if (partial_measure.size() == 1) {
    (*this)(partial_measure.front());
  } else {
    os << "<< ";
    for (size_t voice_index = 0; voice_index < partial_measure.size();
         ++voice_index)
    {
      os << "{"; (*this)(partial_measure[voice_index]); os << "}";
      if (voice_index != partial_measure.size() - 1) os << "\\\\";
    }
    os << " >>";
  }
}

void
generator::operator() (braille::ast::unfolded::partial_voice const &partial_voice)
{
  last_type = rational(), last_dots = 0;
  for (size_t element_index = 0; element_index < partial_voice.size();
       ++element_index)
  {
    apply_visitor(*this, partial_voice[element_index]);
    if (element_index != partial_voice.size() - 1) os << " ";
  }
}

generator::result_type
generator::operator() (braille::ast::barline const &) const
{
}

generator::result_type
generator::operator() (braille::ast::tie const &) const
{
}

generator::result_type
generator::operator() (braille::ast::tuplet_start const &) const
{
}

generator::result_type
generator::operator() (braille::ast::clef const &clef) const
{
  static char const *lily_clef[3][5] = {
    { "french", "G", nullptr, nullptr, nullptr },                  // G-clef
    { "soprano", "mezzosoprano", "C", "tenor", "baritone" },       // C-clef
    { nullptr, nullptr, "varbaritone", "F", "subbass" }            // F-clef
  };
  BOOST_ASSERT(clef.line() > 0);
  if (char const *name = lily_clef[std::size_t(clef.sign())][clef.line() - 1])
    std::cout << "\\clef" << ' ' << name;
  else {
    std::cerr << "Unable to transcribe clef to LilyPond" << std::endl;
    BOOST_ASSERT(false);
  }
}

generator::result_type
generator::operator() (braille::hand_sign const &) const
{
}

generator::result_type
generator::operator() (braille::ast::rest const &rest)
{
  if (rest.whole_measure) {
    os << "R"; if (rest.type) os << "1" << "*" << rest.type;
    last_type = 0, last_dots = 0;
  } else {
    os << "r"; ly_rhythm(rest);
  }

  using braille::ast::notegroup_member_type;
  switch (rest.notegroup_member) {
    case notegroup_member_type::begin:
      os << '[';
      break;
    case notegroup_member_type::end:
      os << ']';
      break;
    default: break;
  }

  if (include_locations) {
    os << "%{" << rest.line << ":" << rest.column << "%}";
  }
}

generator::result_type
generator::operator() (braille::ast::note const &note)
{
  bool grace = false;
  for (articulation const& articulation: note.articulations) {
    switch (articulation) {
    case appoggiatura:             os << "\\appoggiatura "; grace = true; break;
    case short_appoggiatura:       os << "\\acciaccatura "; grace = true; break;
    }
  }
  ly_pitch_step(note.step);
  ly_accidental(note.alter);
  ly_octave(note.octave);
  if (grace and note.type == zero) {
    switch (note.ambiguous_value) {
    case braille::ast::whole_or_16th: os << "16"; break;
    case braille::ast::half_or_32th: os << "32"; break;
    case braille::ast::quarter_or_64th: os << "4"; break;
    case braille::ast::eighth_or_128th: os << "8"; break;
    }
    last_type = 0, last_dots = 0;
  } else ly_rhythm(note);
  if (note.tie) os << "~";
  for (articulation const& articulation: note.articulations) {
    switch (articulation) {
    default:                         break;
    case mordent:                  os << "\\mordent ";      break;
    case turn_above_or_below_note: os << "\\turn ";         break;
    case accent:         os << "->"; break;
    case staccato:       os << "-."; break;
    case staccatissimo:  os << "-|"; break;
    case mezzo_staccato: os << "-_"; break;
    }
  }
  ly_finger(note.fingers);

  using braille::ast::notegroup_member_type;
  switch (note.notegroup_member) {
    case notegroup_member_type::begin:
      os << '[';
      break;
    case notegroup_member_type::end:
      os << ']';
      break;
    default: break;
  }

  switch (note.slur_member) {
  case braille::ast::slur_member_type::begin: os << '('; break;
  case braille::ast::slur_member_type::end:   os << ')'; break;
  }

  if (include_locations) {
    os << "%{" << note.line << ":" << note.column << "%}";
  }
}

generator::result_type
generator::operator() (braille::ast::chord const &chord)
{
  os << "<";
  ly_pitch_step(chord.base.step);
  ly_accidental(chord.base.alter);
  ly_octave(chord.base.octave);
  if (chord.base.tie) os << "~";
  ly_finger(chord.base.fingers);
  for (braille::ast::interval const& interval: chord.intervals) {
    os << " ";
    ly_pitch_step(interval.step);
    ly_accidental(interval.alter);
    ly_octave(interval.octave);
    if (interval.tie) os << "~";
    ly_finger(interval.fingers);
  }
  os << ">";
  ly_rhythm(chord.base);

  using braille::ast::notegroup_member_type;
  switch (chord.base.notegroup_member) {
    case notegroup_member_type::begin:
      os << '[';
      break;
    case notegroup_member_type::end:
      os << ']';
      break;
    default: break;
  }
}

generator::result_type
generator::operator() (braille::ast::moving_note const &chord)
{
  os << "<<{";
  last_type = 0, last_dots = 0;
  (*this)(chord.base);
  os << "}\\\\{";
  // @todo Account for dotted intervals.
  rational const moving_type(chord.base.as_rational() / chord.intervals.size());
  for (braille::ast::interval const& interval: chord.intervals) {
    os << " ";
    ly_pitch_step(interval.step);
    ly_accidental(interval.alter);
    ly_octave(interval.octave);
    os << moving_type.denominator();
    if (moving_type.numerator() != 1) os << '*' << moving_type.numerator();
    if (interval.tie) os << "~";
    ly_finger(interval.fingers);
  }
  os << " }>>";
  last_type = 0, last_dots = 0;         // Next note should have rhythm info
}

void
generator::ly_accidental(int alteration) const
{
  while (alteration < 0) { os << "es"; ++alteration; }
  while (alteration > 0) { os << "is"; --alteration; }
}

void
generator::ly_clef(std::string const& clef) const
{
  os << "\\clef \"" << clef << "\"";
}

namespace {

class print_fingering: public boost::static_visitor<std::ostream &>
{
  std::ostream &os;
public:
  print_fingering(std::ostream &os): os(os) {}
  result_type operator() (braille::finger_change const &change) const
  { return os << "-\"" << change.first << "-" << change.second << "\""; }
  result_type operator() (unsigned finger) const
  { return os << "-" << finger; }
};

}

void
generator::ly_finger(braille::fingering_list const &fingers) const
{
  print_fingering write_to_stream(os);
  for_each(fingers.begin(), fingers.end(), apply_visitor(write_to_stream));
}

void
generator::ly_key(key_signature const &key) const
{
  char const *flats[] = { "f", "bes", "ees", "aes", "des", "ges", "ces" };
  char const *sharps[] = { "g", "d", "a", "e", "b", "fis", "cis" };
  os << "\\key" << " ";
  if (key > 0) os << sharps[key - 1];
  else if (key < 0) os << flats[-key - 1];
  else os << "c";
  os << " " << "\\major";
}

void
generator::ly_rhythm(braille::ast::rhythmic_data const &rhythm)
{
  if (rhythm.type != last_type or rhythm.dots != last_dots) {
    os << rhythm.type.denominator();
    if (rhythm.type.numerator() != 1) os << '*' << rhythm.type.numerator();
    std::fill_n(std::ostream_iterator<char>(os), rhythm.dots, '.');
    last_type = rhythm.type, last_dots = rhythm.dots;
  }
}

void
generator::ly_octave(int octave) const
{
  int const default_octave = 4;
  while (octave > default_octave) { os << "'"; --octave; };
  while (octave < default_octave) { os << ","; ++octave; };
}

void
generator::ly_partial(rational const& duration) const
{
  os << "\\partial" << " " << duration.denominator();
  if (duration.numerator() != 1) os << "*" << duration.numerator();
  os << " ";
}

void
generator::ly_pitch_step(diatonic_step step) const
{
  static char const* steps = "cdefgab";
  os << steps[step];
}

namespace {

struct is_alternative : boost::static_visitor<bool>
{
  result_type operator() (braille::ast::unfolded::measure const &measure) const
  { return measure.ending; }
  result_type operator() (braille::ast::key_and_time_signature const&) const
  { return false; }
};

struct is_measure : boost::static_visitor<bool>
{
  result_type operator() (braille::ast::unfolded::measure const &measure) const
  { return true; }
  result_type operator() (braille::ast::key_and_time_signature const&) const
  { return false; }
};

class has_barline: public boost::static_visitor<bool>
{
  braille::ast::barline type;
public:
  has_barline(braille::ast::barline const &type)
  : type(type)
  {}
  result_type operator() (braille::ast::unfolded::measure const &measure) const
  {
    for (auto const& voice: measure.voices)
      for (auto const& partial_measure: voice)
        for (auto const& partial_voice: partial_measure)
          if (std::any_of( partial_voice.begin(), partial_voice.end()
                         , apply_visitor(*this)
                         )
             )
            return true;
    return false;
  }
  result_type operator() (braille::ast::key_and_time_signature const &) const
  { return false; }

  result_type operator() (braille::ast::barline const &barline) const
  { return barline == type; }
  template<typename T>
  result_type operator() (T const&) const
  { return false; }
};

}

std::size_t
generator::process_repeat_with_alternatives( braille::ast::unfolded::staff const &staff
                                           , std::size_t index
                                           )
{
  std::vector<std::vector<std::size_t>> alternatives;
  std::vector<std::size_t> *indices = nullptr;
  std::size_t i;
  for (i = index; i < staff.size(); ++i) {
    // If this is a time sig followed by a alternative begin, make the time sig part of the alternative
    if (i+1 < staff.size() and
        not apply_visitor(is_measure(), staff[i]) and
        apply_visitor(is_alternative(), staff[i+1])) {
      alternatives.emplace_back();
      indices = &alternatives.back();
      indices->push_back(i++);
      indices->push_back(i);
      continue;
    }
    // Does this element begin an alternative?
    if (apply_visitor(is_alternative(), staff[i])) {
      alternatives.emplace_back();
      indices = &alternatives.back();
    }
    // Is this the start of *another* repeated section?
    if (i > index and apply_visitor(has_barline(braille::ast::begin_repeat), staff[i])) {
      break;
    }
    if (indices) indices->push_back(i);
    if (apply_visitor(has_barline(braille::ast::end_part), staff[i])) {
      ++i; break;
    }
  }

  if (not alternatives.empty()) {
    // We found a section of repeated music with alternative endings.
    os << indent << "  " << "\\repeat volta " << alternatives.size() << " {" << std::endl;
    for (std::size_t measure_index = index;
         measure_index < alternatives.front().front(); ++measure_index)
    {
      braille::ast::unfolded::staff_element const& this_measure = staff[measure_index];

      os << indent << "    "; apply_visitor(*this, this_measure);

      bool barcheck = true;
      repeat_info this_repeat(this_measure);
      if (measure_index < staff.size() - 1) {
        braille::ast::unfolded::staff_element const&
        next_measure = staff[measure_index + 1];
        repeat_info next_repeat(next_measure);
        if (this_repeat.end and next_repeat.begin) {
          os << " " << "\\bar \":|:\"" << " ";
          barcheck = false;
        } else if (next_repeat.begin) {
          os << " " << "\\bar \"|:\"" << " ";
          barcheck = false;
        }
      }
      if (barcheck and this_repeat.end) {
        os << " " << "\\bar \":|\"" << " ";
        barcheck = false;
      }
      if (barcheck) os << " |" << std::endl;
    }
    os << indent << "  " << "}" << std::endl;

    os << indent << "  " << "\\alternative {" << std::endl;
    for (auto indices: alternatives) {
      os << indent << "    " << "{";
      for (std::size_t measure_index: indices) {
        braille::ast::unfolded::staff_element const& this_measure = staff[measure_index];

        apply_visitor(*this, this_measure);
        if (apply_visitor(is_measure(), this_measure)) os << " | ";
      }
      os << "}" << std::endl;
    }
    os << indent << "  " << "}" << std::endl;
    return i;
  }
  return index;
}

std::string instruments[128] = {
  "acoustic grand",
  "bright acoustic",
  "electric grand",
  "honky-tonk",
  "electric piano 1",
  "electric piano 2",
  "harpsichord",
  "clav",
  "celesta",
  "glockenspiel",
  "music box",
  "vibraphone",
  "marimba",
  "xylophone",
  "tubular bells",
  "dulcimer",
  "drawbar organ",
  "percussive organ",
  "rock organ",
  "church organ",
  "reed organ",
  "accordion",
  "harmonica",
  "concertina",
  "acoustic guitar (nylon)",
  "acoustic guitar (steel)",
  "electric guitar (jazz)",
  "electric guitar (clean)",
  "electric guitar (muted)",
  "overdriven guitar",
  "distorted guitar",
  "guitar harmonics",
  "acoustic bass",
  "electric bass (finger)",
  "electric bass (pick)",
  "fretless bass",
  "slap bass 1",
  "slap bass 2",
  "synth bass 1",
  "synth bass 2",
  "violin",
  "viola",
  "cello",
  "contrabass",
  "tremolo strings",
  "pizzicato strings",
  "orchestral harp",
  "timpani",
  "string ensemble 1",
  "string ensemble 2",
  "synthstrings 1",
  "synthstrings 2",
  "choir aahs",
  "voice oohs",
  "synth voice",
  "orchestra hit",
  "trumpet",
  "trombone",
  "tuba",
  "muted trumpet",
  "french horn",
  "brass section",
  "synthbrass 1",
  "synthbrass 2",
  "soprano sax",
  "alto sax",
  "tenor sax",
  "baritone sax",
  "oboe",
  "english horn",
  "bassoon",
  "clarinet",
  "piccolo",
  "flute",
  "recorder",
  "pan flute",
  "blown bottle",
  "shakuhachi",
  "whistle",
  "ocarina",
  "lead 1 (square)",
  "lead 2 (sawtooth)",
  "lead 3 (calliope)",
  "lead 4 (chiff)",
  "lead 5 (charang)",
  "lead 6 (voice)",
  "lead 7 (fifths)",
  "lead 8 (bass+lead)",
  "pad 1 (new age)",
  "pad 2 (warm)",
  "pad 3 (polysynth)",
  "pad 4 (choir)",
  "pad 5 (bowed)",
  "pad 6 (metallic)",
  "pad 7 (halo)",
  "pad 8 (sweep)",
  "fx 1 (rain)",
  "fx 2 (soundtrack)",
  "fx 3 (crystal)",
  "fx 4 (atmosphere)",
  "fx 5 (brightness)",
  "fx 6 (goblins)",
  "fx 7 (echoes)",
  "fx 8 (sci-fi)",
  "sitar",
  "banjo",
  "shamisen",
  "koto",
  "kalimba",
  "bagpipe",
  "fiddle",
  "shanai",
  "tinkle bell",
  "agogo",
  "steel drums",
  "woodblock",
  "taiko drum",
  "melodic tom",
  "synth drum",
  "reverse cymbal",
  "guitar fret noise",
  "breath noise",
  "seashore",
  "bird tweet",
  "telephone ring",
  "helicopter",
  "applause",
  "gunshot"
};

}}
