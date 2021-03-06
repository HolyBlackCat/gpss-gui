#pragma once

#include <cstdint>
#include <utility>

#include "utils/range_set.h"

namespace Unicode
{
    using std::uint32_t;

    using CharRange = std::pair<uint32_t, uint32_t>;
    using CharSet   = RangeSet<uint32_t>;

    namespace Ranges
    {
        // Last version of the range list can be found at: https://www.unicode.org/Public/UCD/latest/ucd/Blocks.txt
        inline constexpr CharRange
            Basic_Latin                                    (0x0000, 0x007F),
            Latin_1_Supplement                             (0x0080, 0x00FF),
            Latin_Extended_A                               (0x0100, 0x017F),
            Latin_Extended_B                               (0x0180, 0x024F),
            IPA_Extensions                                 (0x0250, 0x02AF),
            Spacing_Modifier_Letters                       (0x02B0, 0x02FF),
            Combining_Diacritical_Marks                    (0x0300, 0x036F),
            Greek_and_Coptic                               (0x0370, 0x03FF),
            Cyrillic                                       (0x0400, 0x04FF),
            Cyrillic_Supplement                            (0x0500, 0x052F),
            Armenian                                       (0x0530, 0x058F),
            Hebrew                                         (0x0590, 0x05FF),
            Arabic                                         (0x0600, 0x06FF),
            Syriac                                         (0x0700, 0x074F),
            Arabic_Supplement                              (0x0750, 0x077F),
            Thaana                                         (0x0780, 0x07BF),
            NKo                                            (0x07C0, 0x07FF),
            Samaritan                                      (0x0800, 0x083F),
            Mandaic                                        (0x0840, 0x085F),
            Syriac_Supplement                              (0x0860, 0x086F),
            Arabic_Extended_A                              (0x08A0, 0x08FF),
            Devanagari                                     (0x0900, 0x097F),
            Bengali                                        (0x0980, 0x09FF),
            Gurmukhi                                       (0x0A00, 0x0A7F),
            Gujarati                                       (0x0A80, 0x0AFF),
            Oriya                                          (0x0B00, 0x0B7F),
            Tamil                                          (0x0B80, 0x0BFF),
            Telugu                                         (0x0C00, 0x0C7F),
            Kannada                                        (0x0C80, 0x0CFF),
            Malayalam                                      (0x0D00, 0x0D7F),
            Sinhala                                        (0x0D80, 0x0DFF),
            Thai                                           (0x0E00, 0x0E7F),
            Lao                                            (0x0E80, 0x0EFF),
            Tibetan                                        (0x0F00, 0x0FFF),
            Myanmar                                        (0x1000, 0x109F),
            Georgian                                       (0x10A0, 0x10FF),
            Hangul_Jamo                                    (0x1100, 0x11FF),
            Ethiopic                                       (0x1200, 0x137F),
            Ethiopic_Supplement                            (0x1380, 0x139F),
            Cherokee                                       (0x13A0, 0x13FF),
            Unified_Canadian_Aboriginal_Syllabics          (0x1400, 0x167F),
            Ogham                                          (0x1680, 0x169F),
            Runic                                          (0x16A0, 0x16FF),
            Tagalog                                        (0x1700, 0x171F),
            Hanunoo                                        (0x1720, 0x173F),
            Buhid                                          (0x1740, 0x175F),
            Tagbanwa                                       (0x1760, 0x177F),
            Khmer                                          (0x1780, 0x17FF),
            Mongolian                                      (0x1800, 0x18AF),
            Unified_Canadian_Aboriginal_Syllabics_Extended (0x18B0, 0x18FF),
            Limbu                                          (0x1900, 0x194F),
            Tai_Le                                         (0x1950, 0x197F),
            New_Tai_Lue                                    (0x1980, 0x19DF),
            Khmer_Symbols                                  (0x19E0, 0x19FF),
            Buginese                                       (0x1A00, 0x1A1F),
            Tai_Tham                                       (0x1A20, 0x1AAF),
            Combining_Diacritical_Marks_Extended           (0x1AB0, 0x1AFF),
            Balinese                                       (0x1B00, 0x1B7F),
            Sundanese                                      (0x1B80, 0x1BBF),
            Batak                                          (0x1BC0, 0x1BFF),
            Lepcha                                         (0x1C00, 0x1C4F),
            Ol_Chiki                                       (0x1C50, 0x1C7F),
            Cyrillic_Extended_C                            (0x1C80, 0x1C8F),
            Georgian_Extended                              (0x1C90, 0x1CBF),
            Sundanese_Supplement                           (0x1CC0, 0x1CCF),
            Vedic_Extensions                               (0x1CD0, 0x1CFF),
            Phonetic_Extensions                            (0x1D00, 0x1D7F),
            Phonetic_Extensions_Supplement                 (0x1D80, 0x1DBF),
            Combining_Diacritical_Marks_Supplement         (0x1DC0, 0x1DFF),
            Latin_Extended_Additional                      (0x1E00, 0x1EFF),
            Greek_Extended                                 (0x1F00, 0x1FFF),
            General_Punctuation                            (0x2000, 0x206F),
            Superscripts_and_Subscripts                    (0x2070, 0x209F),
            Currency_Symbols                               (0x20A0, 0x20CF),
            Combining_Diacritical_Marks_for_Symbols        (0x20D0, 0x20FF),
            Letterlike_Symbols                             (0x2100, 0x214F),
            Number_Forms                                   (0x2150, 0x218F),
            Arrows                                         (0x2190, 0x21FF),
            Mathematical_Operators                         (0x2200, 0x22FF),
            Miscellaneous_Technical                        (0x2300, 0x23FF),
            Control_Pictures                               (0x2400, 0x243F),
            Optical_Character_Recognition                  (0x2440, 0x245F),
            Enclosed_Alphanumerics                         (0x2460, 0x24FF),
            Box_Drawing                                    (0x2500, 0x257F),
            Block_Elements                                 (0x2580, 0x259F),
            Geometric_Shapes                               (0x25A0, 0x25FF),
            Miscellaneous_Symbols                          (0x2600, 0x26FF),
            Dingbats                                       (0x2700, 0x27BF),
            Miscellaneous_Mathematical_Symbols_A           (0x27C0, 0x27EF),
            Supplemental_Arrows_A                          (0x27F0, 0x27FF),
            Braille_Patterns                               (0x2800, 0x28FF),
            Supplemental_Arrows_B                          (0x2900, 0x297F),
            Miscellaneous_Mathematical_Symbols_B           (0x2980, 0x29FF),
            Supplemental_Mathematical_Operators            (0x2A00, 0x2AFF),
            Miscellaneous_Symbols_and_Arrows               (0x2B00, 0x2BFF),
            Glagolitic                                     (0x2C00, 0x2C5F),
            Latin_Extended_C                               (0x2C60, 0x2C7F),
            Coptic                                         (0x2C80, 0x2CFF),
            Georgian_Supplement                            (0x2D00, 0x2D2F),
            Tifinagh                                       (0x2D30, 0x2D7F),
            Ethiopic_Extended                              (0x2D80, 0x2DDF),
            Cyrillic_Extended_A                            (0x2DE0, 0x2DFF),
            Supplemental_Punctuation                       (0x2E00, 0x2E7F),
            CJK_Radicals_Supplement                        (0x2E80, 0x2EFF),
            Kangxi_Radicals                                (0x2F00, 0x2FDF),
            Ideographic_Description_Characters             (0x2FF0, 0x2FFF),
            CJK_Symbols_and_Punctuation                    (0x3000, 0x303F),
            Hiragana                                       (0x3040, 0x309F),
            Katakana                                       (0x30A0, 0x30FF),
            Bopomofo                                       (0x3100, 0x312F),
            Hangul_Compatibility_Jamo                      (0x3130, 0x318F),
            Kanbun                                         (0x3190, 0x319F),
            Bopomofo_Extended                              (0x31A0, 0x31BF),
            CJK_Strokes                                    (0x31C0, 0x31EF),
            Katakana_Phonetic_Extensions                   (0x31F0, 0x31FF),
            Enclosed_CJK_Letters_and_Months                (0x3200, 0x32FF),
            CJK_Compatibility                              (0x3300, 0x33FF),
            CJK_Unified_Ideographs_Extension_A             (0x3400, 0x4DBF),
            Yijing_Hexagram_Symbols                        (0x4DC0, 0x4DFF),
            CJK_Unified_Ideographs                         (0x4E00, 0x9FFF),
            Yi_Syllables                                   (0xA000, 0xA48F),
            Yi_Radicals                                    (0xA490, 0xA4CF),
            Lisu                                           (0xA4D0, 0xA4FF),
            Vai                                            (0xA500, 0xA63F),
            Cyrillic_Extended_B                            (0xA640, 0xA69F),
            Bamum                                          (0xA6A0, 0xA6FF),
            Modifier_Tone_Letters                          (0xA700, 0xA71F),
            Latin_Extended_D                               (0xA720, 0xA7FF),
            Syloti_Nagri                                   (0xA800, 0xA82F),
            Common_Indic_Number_Forms                      (0xA830, 0xA83F),
            Phags_pa                                       (0xA840, 0xA87F),
            Saurashtra                                     (0xA880, 0xA8DF),
            Devanagari_Extended                            (0xA8E0, 0xA8FF),
            Kayah_Li                                       (0xA900, 0xA92F),
            Rejang                                         (0xA930, 0xA95F),
            Hangul_Jamo_Extended_A                         (0xA960, 0xA97F),
            Javanese                                       (0xA980, 0xA9DF),
            Myanmar_Extended_B                             (0xA9E0, 0xA9FF),
            Cham                                           (0xAA00, 0xAA5F),
            Myanmar_Extended_A                             (0xAA60, 0xAA7F),
            Tai_Viet                                       (0xAA80, 0xAADF),
            Meetei_Mayek_Extensions                        (0xAAE0, 0xAAFF),
            Ethiopic_Extended_A                            (0xAB00, 0xAB2F),
            Latin_Extended_E                               (0xAB30, 0xAB6F),
            Cherokee_Supplement                            (0xAB70, 0xABBF),
            Meetei_Mayek                                   (0xABC0, 0xABFF),
            Hangul_Syllables                               (0xAC00, 0xD7AF),
            Hangul_Jamo_Extended_B                         (0xD7B0, 0xD7FF),
            High_Surrogates                                (0xD800, 0xDB7F),
            High_Private_Use_Surrogates                    (0xDB80, 0xDBFF),
            Low_Surrogates                                 (0xDC00, 0xDFFF),
            Private_Use_Area                               (0xE000, 0xF8FF),
            CJK_Compatibility_Ideographs                   (0xF900, 0xFAFF),
            Alphabetic_Presentation_Forms                  (0xFB00, 0xFB4F),
            Arabic_Presentation_Forms_A                    (0xFB50, 0xFDFF),
            Variation_Selectors                            (0xFE00, 0xFE0F),
            Vertical_Forms                                 (0xFE10, 0xFE1F),
            Combining_Half_Marks                           (0xFE20, 0xFE2F),
            CJK_Compatibility_Forms                        (0xFE30, 0xFE4F),
            Small_Form_Variants                            (0xFE50, 0xFE6F),
            Arabic_Presentation_Forms_B                    (0xFE70, 0xFEFF),
            Halfwidth_and_Fullwidth_Forms                  (0xFF00, 0xFFEF),
            Specials                                       (0xFFF0, 0xFFFF),
            Linear_B_Syllabary                             (0x10000, 0x1007F),
            Linear_B_Ideograms                             (0x10080, 0x100FF),
            Aegean_Numbers                                 (0x10100, 0x1013F),
            Ancient_Greek_Numbers                          (0x10140, 0x1018F),
            Ancient_Symbols                                (0x10190, 0x101CF),
            Phaistos_Disc                                  (0x101D0, 0x101FF),
            Lycian                                         (0x10280, 0x1029F),
            Carian                                         (0x102A0, 0x102DF),
            Coptic_Epact_Numbers                           (0x102E0, 0x102FF),
            Old_Italic                                     (0x10300, 0x1032F),
            Gothic                                         (0x10330, 0x1034F),
            Old_Permic                                     (0x10350, 0x1037F),
            Ugaritic                                       (0x10380, 0x1039F),
            Old_Persian                                    (0x103A0, 0x103DF),
            Deseret                                        (0x10400, 0x1044F),
            Shavian                                        (0x10450, 0x1047F),
            Osmanya                                        (0x10480, 0x104AF),
            Osage                                          (0x104B0, 0x104FF),
            Elbasan                                        (0x10500, 0x1052F),
            Caucasian_Albanian                             (0x10530, 0x1056F),
            Linear_A                                       (0x10600, 0x1077F),
            Cypriot_Syllabary                              (0x10800, 0x1083F),
            Imperial_Aramaic                               (0x10840, 0x1085F),
            Palmyrene                                      (0x10860, 0x1087F),
            Nabataean                                      (0x10880, 0x108AF),
            Hatran                                         (0x108E0, 0x108FF),
            Phoenician                                     (0x10900, 0x1091F),
            Lydian                                         (0x10920, 0x1093F),
            Meroitic_Hieroglyphs                           (0x10980, 0x1099F),
            Meroitic_Cursive                               (0x109A0, 0x109FF),
            Kharoshthi                                     (0x10A00, 0x10A5F),
            Old_South_Arabian                              (0x10A60, 0x10A7F),
            Old_North_Arabian                              (0x10A80, 0x10A9F),
            Manichaean                                     (0x10AC0, 0x10AFF),
            Avestan                                        (0x10B00, 0x10B3F),
            Inscriptional_Parthian                         (0x10B40, 0x10B5F),
            Inscriptional_Pahlavi                          (0x10B60, 0x10B7F),
            Psalter_Pahlavi                                (0x10B80, 0x10BAF),
            Old_Turkic                                     (0x10C00, 0x10C4F),
            Old_Hungarian                                  (0x10C80, 0x10CFF),
            Hanifi_Rohingya                                (0x10D00, 0x10D3F),
            Rumi_Numeral_Symbols                           (0x10E60, 0x10E7F),
            Old_Sogdian                                    (0x10F00, 0x10F2F),
            Sogdian                                        (0x10F30, 0x10F6F),
            Brahmi                                         (0x11000, 0x1107F),
            Kaithi                                         (0x11080, 0x110CF),
            Sora_Sompeng                                   (0x110D0, 0x110FF),
            Chakma                                         (0x11100, 0x1114F),
            Mahajani                                       (0x11150, 0x1117F),
            Sharada                                        (0x11180, 0x111DF),
            Sinhala_Archaic_Numbers                        (0x111E0, 0x111FF),
            Khojki                                         (0x11200, 0x1124F),
            Multani                                        (0x11280, 0x112AF),
            Khudawadi                                      (0x112B0, 0x112FF),
            Grantha                                        (0x11300, 0x1137F),
            Newa                                           (0x11400, 0x1147F),
            Tirhuta                                        (0x11480, 0x114DF),
            Siddham                                        (0x11580, 0x115FF),
            Modi                                           (0x11600, 0x1165F),
            Mongolian_Supplement                           (0x11660, 0x1167F),
            Takri                                          (0x11680, 0x116CF),
            Ahom                                           (0x11700, 0x1173F),
            Dogra                                          (0x11800, 0x1184F),
            Warang_Citi                                    (0x118A0, 0x118FF),
            Zanabazar_Square                               (0x11A00, 0x11A4F),
            Soyombo                                        (0x11A50, 0x11AAF),
            Pau_Cin_Hau                                    (0x11AC0, 0x11AFF),
            Bhaiksuki                                      (0x11C00, 0x11C6F),
            Marchen                                        (0x11C70, 0x11CBF),
            Masaram_Gondi                                  (0x11D00, 0x11D5F),
            Gunjala_Gondi                                  (0x11D60, 0x11DAF),
            Makasar                                        (0x11EE0, 0x11EFF),
            Cuneiform                                      (0x12000, 0x123FF),
            Cuneiform_Numbers_and_Punctuation              (0x12400, 0x1247F),
            Early_Dynastic_Cuneiform                       (0x12480, 0x1254F),
            Egyptian_Hieroglyphs                           (0x13000, 0x1342F),
            Anatolian_Hieroglyphs                          (0x14400, 0x1467F),
            Bamum_Supplement                               (0x16800, 0x16A3F),
            Mro                                            (0x16A40, 0x16A6F),
            Bassa_Vah                                      (0x16AD0, 0x16AFF),
            Pahawh_Hmong                                   (0x16B00, 0x16B8F),
            Medefaidrin                                    (0x16E40, 0x16E9F),
            Miao                                           (0x16F00, 0x16F9F),
            Ideographic_Symbols_and_Punctuation            (0x16FE0, 0x16FFF),
            Tangut                                         (0x17000, 0x187FF),
            Tangut_Components                              (0x18800, 0x18AFF),
            Kana_Supplement                                (0x1B000, 0x1B0FF),
            Kana_Extended_A                                (0x1B100, 0x1B12F),
            Nushu                                          (0x1B170, 0x1B2FF),
            Duployan                                       (0x1BC00, 0x1BC9F),
            Shorthand_Format_Controls                      (0x1BCA0, 0x1BCAF),
            Byzantine_Musical_Symbols                      (0x1D000, 0x1D0FF),
            Musical_Symbols                                (0x1D100, 0x1D1FF),
            Ancient_Greek_Musical_Notation                 (0x1D200, 0x1D24F),
            Mayan_Numerals                                 (0x1D2E0, 0x1D2FF),
            Tai_Xuan_Jing_Symbols                          (0x1D300, 0x1D35F),
            Counting_Rod_Numerals                          (0x1D360, 0x1D37F),
            Mathematical_Alphanumeric_Symbols              (0x1D400, 0x1D7FF),
            Sutton_SignWriting                             (0x1D800, 0x1DAAF),
            Glagolitic_Supplement                          (0x1E000, 0x1E02F),
            Mende_Kikakui                                  (0x1E800, 0x1E8DF),
            Adlam                                          (0x1E900, 0x1E95F),
            Indic_Siyaq_Numbers                            (0x1EC70, 0x1ECBF),
            Arabic_Mathematical_Alphabetic_Symbols         (0x1EE00, 0x1EEFF),
            Mahjong_Tiles                                  (0x1F000, 0x1F02F),
            Domino_Tiles                                   (0x1F030, 0x1F09F),
            Playing_Cards                                  (0x1F0A0, 0x1F0FF),
            Enclosed_Alphanumeric_Supplement               (0x1F100, 0x1F1FF),
            Enclosed_Ideographic_Supplement                (0x1F200, 0x1F2FF),
            Miscellaneous_Symbols_and_Pictographs          (0x1F300, 0x1F5FF),
            Emoticons                                      (0x1F600, 0x1F64F),
            Ornamental_Dingbats                            (0x1F650, 0x1F67F),
            Transport_and_Map_Symbols                      (0x1F680, 0x1F6FF),
            Alchemical_Symbols                             (0x1F700, 0x1F77F),
            Geometric_Shapes_Extended                      (0x1F780, 0x1F7FF),
            Supplemental_Arrows_C                          (0x1F800, 0x1F8FF),
            Supplemental_Symbols_and_Pictographs           (0x1F900, 0x1F9FF),
            Chess_Symbols                                  (0x1FA00, 0x1FA6F),
            CJK_Unified_Ideographs_Extension_B             (0x20000, 0x2A6DF),
            CJK_Unified_Ideographs_Extension_C             (0x2A700, 0x2B73F),
            CJK_Unified_Ideographs_Extension_D             (0x2B740, 0x2B81F),
            CJK_Unified_Ideographs_Extension_E             (0x2B820, 0x2CEAF),
            CJK_Unified_Ideographs_Extension_F             (0x2CEB0, 0x2EBEF),
            CJK_Compatibility_Ideographs_Supplement        (0x2F800, 0x2FA1F),
            Tags                                           (0xE0000, 0xE007F),
            Variation_Selectors_Supplement                 (0xE0100, 0xE01EF),
            Supplementary_Private_Use_Area_A               (0xF0000, 0xFFFFF),
            Supplementary_Private_Use_Area_B               (0x100000, 0x10FFFF);
    }
}
