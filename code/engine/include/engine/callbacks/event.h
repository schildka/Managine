#pragma once

#include <functional>
#include <algorithm>
#include <list>

namespace engine::callbacks {

    /**
     * An event that users can listen to by placing a callback.
     * @tparam Owner Only the owner of an event can fire the event.
     * @tparam Payload The types of payload (arguments) for callbacks.
     */
    template<typename Owner, typename ... Payload>
    struct event {
        using  callback_t = typename std::function<void( Payload ...)>;
        using  callback_container_t = typename std::list<callback_t>;
        using  handle_t = typename callback_container_t::iterator;

        /**
         * Supplied callbacks will be executed when the event is fired.
         * @param callback The event handler.
         * @return An iterator that must be used when unsubscribing from this event.
         */
        handle_t subscribe(callback_t callback) {
            callbacks.push_back(callback);
            return --callbacks.end();
        }

        /**
         * The supplied handle will be used to remove the previously installed callback.
         * @param callback_handle The handle that was returned by subscribe() when adding the callback.
         */
        void unsubscribe(handle_t callback_handle) {
            callbacks.erase(callback_handle);
        }

    private:
        using owner_t = Owner;
        friend owner_t;

        event() = default;
        ~event() = default;

        void fire(Payload ... payload) {
            // callbacks might be added/removed by callbacks
            callback_container_t unmodified_callbacks(callbacks);
            for (auto cb : unmodified_callbacks)
                cb(std::forward<Payload>(payload) ...);
        }

        callback_container_t callbacks;
    };

}
