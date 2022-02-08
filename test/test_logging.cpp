/***************************************************************************
 *            test_logging.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 * This file is part of CONCLOG, under the MIT license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <list>
#include "thread.hpp"
#include "logging.hpp"
#include "progress_indicator.hpp"
#include "test.hpp"

using namespace ConcLog;

void sample_function() {
    CONCLOG_SCOPE_CREATE
    CONCLOG_PRINTLN("val=inf, x0=2.0^3*1.32424242432423[2,3], y>[0.1:0.2] (z={0:1}), 1, x0, x11, true@1.")
}

void sample_printhold_simple_loop(std::string txt, SizeType u) {
    CONCLOG_SCOPE_CREATE
    for (unsigned int i=0; i<3; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(u));
        CONCLOG_SCOPE_PRINTHOLD(txt<<"@"<<i)
    }
}

void sample_printhold_nested_loop(std::string txt, SizeType u) {
    CONCLOG_SCOPE_CREATE
    for (unsigned int i=0; i<3; ++i) {
        sample_printhold_simple_loop("internal",u);
        std::this_thread::sleep_for(std::chrono::milliseconds(u));
        CONCLOG_SCOPE_PRINTHOLD(txt<<"@"<<i)
    }
}

void print_something1() {
    CONCLOG_PRINTLN("This is a call from thread id " << std::this_thread::get_id() << " named '" << ConcLogger::instance().current_thread_name() << "'")
}

void print_something2() {
    CONCLOG_SCOPE_CREATE
    CONCLOG_PRINTLN("This is a call from thread id " << std::this_thread::get_id() << " named '" << ConcLogger::instance().current_thread_name() << "'")
}

class TestLogging {
  public:

    TestLogging() {
        ConcLogger::instance().configuration().set_prints_level_on_change_only(false);
    }

    void test() {
        CONCLOG_TEST_CALL(test_print_configuration())
        CONCLOG_TEST_CALL(test_shown_single_print())
        CONCLOG_TEST_CALL(test_hidden_single_print())
        CONCLOG_TEST_CALL(test_muted_print())
        CONCLOG_TEST_CALL(test_use_blocking_scheduler())
        CONCLOG_TEST_CALL(test_use_nonblocking_scheduler())
        CONCLOG_TEST_CALL(test_shown_call_function_with_entrance_and_exit())
        CONCLOG_TEST_CALL(test_hide_call_function_with_entrance_and_exit())
        CONCLOG_TEST_CALL(test_indents_based_on_level())
        CONCLOG_TEST_CALL(test_hold_line())
        CONCLOG_TEST_CALL(test_hold_line_with_newline_println())
        CONCLOG_TEST_CALL(test_hold_long_line())
        CONCLOG_TEST_CALL(test_hold_multiple())
        CONCLOG_TEST_CALL(test_light_theme())
        CONCLOG_TEST_CALL(test_dark_theme())
        CONCLOG_TEST_CALL(test_theme_custom_keyword())
        CONCLOG_TEST_CALL(test_theme_bgcolor_bold_underline())
        CONCLOG_TEST_CALL(test_handles_multiline_output())
        CONCLOG_TEST_CALL(test_discards_newlines_and_indentation())
        CONCLOG_TEST_CALL(test_redirect())
        CONCLOG_TEST_CALL(test_multiple_threads_with_blocking_scheduler())
        CONCLOG_TEST_CALL(test_multiple_threads_with_nonblocking_scheduler())
        CONCLOG_TEST_CALL(test_register_self_thread())
        CONCLOG_TEST_CALL(test_printing_policy_with_theme_and_print_level(true,true))
        CONCLOG_TEST_CALL(test_printing_policy_with_theme_and_print_level(true,false))
        CONCLOG_TEST_CALL(test_printing_policy_with_theme_and_print_level(false,false))
        CONCLOG_TEST_CALL(test_printing_policy_with_theme_and_print_level(false,true))
    }

    void test_print_configuration() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        CONCLOG_PRINTLN(ConcLogger::instance().configuration())
    }

    void test_shown_single_print() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        CONCLOG_PRINTLN("This is a call on level 1")
    }

    void test_hidden_single_print() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(0);
        CONCLOG_PRINTLN("This is a hidden call on level 1")
    }

    void test_muted_print() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        CONCLOG_PRINTLN("This is the first line shown.")
        CONCLOG_RUN_MUTED(print_something1())
        CONCLOG_PRINTLN("This is the second and last line shown.")
    }

    void test_use_blocking_scheduler() {
        ConcLogger::instance().use_blocking_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        CONCLOG_PRINTLN("This is a call")
        CONCLOG_PRINTLN("This is another call")
    }

    void test_use_nonblocking_scheduler() {
        ConcLogger::instance().use_nonblocking_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        CONCLOG_PRINTLN("This is a call")
        CONCLOG_PRINTLN("This is another call")
    }

    void test_shown_call_function_with_entrance_and_exit() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_prints_scope_entrance(true);
        ConcLogger::instance().configuration().set_prints_scope_exit(true);
        CONCLOG_PRINTLN("This is a call on level 1");
        CONCLOG_RUN_AT(0,sample_function());
        CONCLOG_PRINTLN("This is again a call on level 1");
    }

    void test_hide_call_function_with_entrance_and_exit() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_prints_scope_entrance(false);
        ConcLogger::instance().configuration().set_prints_scope_exit(false);
        CONCLOG_PRINTLN("This is a call on level 1");
        CONCLOG_RUN_AT(1,sample_function());
        CONCLOG_PRINTLN("This is again a call on level 1");
    }

    void test_indents_based_on_level() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_indents_based_on_level(true);
        CONCLOG_PRINTLN("Call at level 1");
        CONCLOG_PRINTLN_AT(1,"Call at level 2");
        ConcLogger::instance().configuration().set_indents_based_on_level(false);
        CONCLOG_PRINTLN("Call at level 1");
        CONCLOG_PRINTLN_AT(1,"Call at level 2");
    }

    void test_handles_multiline_output() {
        SizeType num_cols = ConcLogger::instance().get_window_columns();
        CONCLOG_PRINT_TEST_COMMENT("Number of window columns: " << num_cols)
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_handles_multiline_output(true);
        CONCLOG_PRINTLN("<begin>" << std::string(num_cols,' ') << "<end>")
        ConcLogger::instance().configuration().set_handles_multiline_output(false);
        CONCLOG_PRINTLN("<begin>" << std::string(num_cols,' ') << "<end>")
    }

    void test_discards_newlines_and_indentation() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_discards_newlines_and_indentation(true);
        CONCLOG_PRINTLN("This text should just be in a single line \n       with no extra whitespaces.");
        ConcLogger::instance().configuration().set_discards_newlines_and_indentation(false);
        CONCLOG_PRINTLN("This text should be in two lines\n          where the second one starts several whitespaces in.");
    }

    void _hold_short_line() {
        CONCLOG_SCOPE_CREATE;
        ProgressIndicator indicator(10.0);
        for (unsigned int i=0; i<10; ++i) {
            indicator.update_current(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            CONCLOG_SCOPE_PRINTHOLD("[" << indicator.symbol() << "] " << indicator.percentage() << "%");
        }
    }

    void test_hold_line() {
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().use_immediate_scheduler();
        _hold_short_line();
        ConcLogger::instance().use_blocking_scheduler();
        _hold_short_line();
        ConcLogger::instance().use_nonblocking_scheduler();
        _hold_short_line();
    }

    void test_hold_line_with_newline_println() {
        ConcLogger::instance().use_immediate_scheduler();
        CONCLOG_SCOPE_CREATE;
        ConcLogger::instance().configuration().set_verbosity(2);
        ProgressIndicator indicator(10.0);
        for (unsigned int i=0; i<10; ++i) {
            indicator.update_current(i);
            CONCLOG_PRINTLN("first line\nsecond line")
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            CONCLOG_SCOPE_PRINTHOLD("[" << indicator.symbol() << "] " << indicator.percentage() << "%");
        }
    }

    void test_hold_long_line() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        CONCLOG_SCOPE_CREATE;
        SizeType num_cols = ConcLogger::instance().get_window_columns();

        std::string exactly_str(num_cols-4,'x'); // exactly the length required to fill the columns (given a prefix of 4 chars)
        std::string larger_str(num_cols,'x'); // larger enough

        for (unsigned int i=0; i<10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            CONCLOG_PRINTLN("i="<<i)
            CONCLOG_SCOPE_PRINTHOLD(exactly_str);
        }

        for (unsigned int i=0; i<10; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            CONCLOG_PRINTLN("i="<<i)
            CONCLOG_SCOPE_PRINTHOLD(larger_str);
        }
    }

    void test_hold_multiple() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(4);
        CONCLOG_SCOPE_CREATE;
        SizeType u=30;
        for (unsigned int i=0; i<3; ++i) {
            sample_printhold_nested_loop("intermediate",u);
            std::this_thread::sleep_for(std::chrono::milliseconds(u));
            CONCLOG_SCOPE_PRINTHOLD("external@"<<i);
        }
    }

    void test_light_theme() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_theme(TT_THEME_LIGHT);
        std::clog << TT_THEME_LIGHT << std::endl;
        CONCLOG_PRINTLN("This is a call on level 1")
        CONCLOG_RUN_AT(0,sample_function())
        CONCLOG_PRINTLN("This is again a call on level 1")
        ConcLogger::instance().configuration().set_theme(TT_THEME_NONE);
    }

    void test_dark_theme() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(2);
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        std::clog << TT_THEME_DARK << std::endl;
        CONCLOG_PRINTLN("This is a call on level 1")
        CONCLOG_RUN_AT(0,sample_function())
        CONCLOG_PRINTLN("This is again a call on level 1")
    }

    void test_theme_custom_keyword() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        ConcLogger::instance().configuration().add_custom_keyword("first");
        ConcLogger::instance().configuration().add_custom_keyword("second",TT_STYLE_DARKORANGE);
        CONCLOG_PRINTLN("This is a default first, a styled second, an ignored secondsecond and msecond and second1 and 1second and firstsecond but not ignored [second and second]")
    }

    void test_theme_bgcolor_bold_underline() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        std::list<TerminalTextStyle> styles;
        styles.push_back(TerminalTextStyle(0,0,true,false));
        styles.push_back(TerminalTextStyle(0,0,false,true));
        styles.push_back(TerminalTextStyle(0,0,true,true));
        styles.push_back(TerminalTextStyle(0,88,false,false));
        styles.push_back(TerminalTextStyle(0,88,true,false));
        styles.push_back(TerminalTextStyle(0,88,false,true));
        styles.push_back(TerminalTextStyle(0,88,true,true));
        std::ostringstream ss;
        for (auto style: styles) {
            ss << style() << "x" << TerminalTextStyle::RESET << " ";
        }
        std::clog << ss.str() << std::endl;
    }

    void test_redirect() {
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().configuration().set_verbosity(1);
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        CONCLOG_PRINTLN("This is call 1");
        ConcLogger::instance().redirect_to_file("log.txt");
        CONCLOG_PRINTLN("This is call 2");
        CONCLOG_PRINTLN("This is call 3");
        ConcLogger::instance().redirect_to_console();
        CONCLOG_PRINTLN("This is call 4");
        CONCLOG_PRINTLN("This is call 5");

        std::string line;
        std::ifstream file("log.txt");
        unsigned int count = 0;
        if(file.is_open()) {
            while(!file.eof()) {
                getline(file,line);
                count++;
            }
            file.close();
        }
        CONCLOG_TEST_EQUALS(count,3);
    }

    void test_multiple_threads_with_blocking_scheduler() {
        ConcLogger::instance().use_blocking_scheduler();
        ConcLogger::instance().configuration().set_verbosity(3);
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        CONCLOG_PRINTLN("Printing on the " << ConcLogger::instance().current_thread_name() << " thread without other threads");
        CONCLOG_TEST_EQUALS(ConcLogger::instance().cached_last_printed_thread_name().compare("main"),0);
        Thread thread1([] { print_something1(); },"thr1");
        Thread thread2([] { print_something2(); },"thr2");
        CONCLOG_PRINTLN("Printing again on the main thread, but with other threads");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        CONCLOG_TEST_PRINT(ConcLogger::instance().cached_last_printed_thread_name());
        CONCLOG_TEST_ASSERT(ConcLogger::instance().cached_last_printed_thread_name().compare("thr1") == 0 or
                                    ConcLogger::instance().cached_last_printed_thread_name().compare("thr2") == 0);
    }

    void test_multiple_threads_with_nonblocking_scheduler() {
        ConcLogger::instance().use_nonblocking_scheduler();
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        ConcLogger::instance().configuration().set_verbosity(3);
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);

        CONCLOG_PRINTLN("Printing on the " << ConcLogger::instance().current_thread_name() << " thread without other threads");
        Thread thread1([] { print_something1(); });
        Thread thread2([] { print_something1(); });
        Thread thread3([] { print_something1(); });
        Thread thread4([] { print_something1(); });
        Thread thread5([] { print_something1(); });
        Thread thread6([] { print_something1(); });
        CONCLOG_PRINTLN("Printing again on the main thread, but with other threads");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    void test_register_self_thread() {
        ConcLogger::instance().use_blocking_scheduler();
        ConcLogger::instance().configuration().set_verbosity(3);
        ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        CONCLOG_PRINTLN("Printing on the " << ConcLogger::instance().current_thread_name() << " thread without other threads");
        CONCLOG_TEST_EQUALS(ConcLogger::instance().cached_last_printed_thread_name().compare("main"),0);
        std::thread::id thread_id;
        std::thread thread1([&thread_id] { thread_id = std::this_thread::get_id(); ConcLogger::instance().register_self_thread("thr1",1); print_something1(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        thread1.join();
        ConcLogger::instance().unregister_thread(thread_id);
    }

    void test_printing_policy_with_theme_and_print_level(bool use_theme, bool print_level) {
        CONCLOG_PRINT_TEST_COMMENT("Policies: " << ThreadNamePrintingPolicy::BEFORE << " " << ThreadNamePrintingPolicy::AFTER << " " << ThreadNamePrintingPolicy::NEVER)
        ConcLogger::instance().use_immediate_scheduler();
        ConcLogger::instance().use_blocking_scheduler();
        ConcLogger::instance().configuration().set_verbosity(3);
        if (use_theme) ConcLogger::instance().configuration().set_theme(TT_THEME_DARK);
        else ConcLogger::instance().configuration().set_theme(TT_THEME_NONE);
        ConcLogger::instance().configuration().set_prints_level_on_change_only(print_level);
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::BEFORE);
        Thread thread1([] { print_something1(); },"thr1");
        CONCLOG_PRINTLN("Printing thread name before");
        CONCLOG_PRINTLN("Printing thread name before again");
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::AFTER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        Thread thread2([] { print_something1(); },"thr2");
        CONCLOG_PRINTLN("Printing thread name after");
        CONCLOG_PRINTLN("Printing thread name after again");
        ConcLogger::instance().configuration().set_thread_name_printing_policy(ThreadNamePrintingPolicy::NEVER);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        CONCLOG_PRINTLN("Printing thread name never");
        Thread thread3([] { print_something1(); },"thr3");
    }
};

int main() {

    TestLogging().test();

    return CONCLOG_TEST_FAILURES;
}

