/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <eosiolib/transaction.h>
#include <eosiolib/action.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/types.hpp>
#include <eosiolib/serialize.hpp>
#include <vector>

namespace eosio {

   /**
    * @defgroup transactioncppapi Transaction C++ API
    * @ingroup transactionapi
    * @brief Type-safe C++ wrappers for transaction C API
    *
    * @note There are some methods from the @ref transactioncapi that can be used directly from C++
    *
    * @{
    */

   class transaction {
   public:
      transaction(time exp = now() + 60, region_id r = 0)
      :expiration(exp),region(r)
      {}

      void send(uint64_t sender_id, account_name payer, time delay_until = now()) const {
         auto serialize = pack(*this);
         send_deferred(sender_id, payer, delay_until, serialize.data(), serialize.size());
      }

      time            expiration;
      region_id       region;
      uint16_t        ref_block_num;
      uint32_t        ref_block_prefix;
      unsigned_int    net_usage_words = 0UL; /// number of 8 byte words this transaction can serialize into after compressions
      unsigned_int    kcpu_usage = 0UL; /// number of CPU usage units to bill transaction for
      unsigned_int    delay_sec = 0UL; /// number of CPU usage units to bill transaction for

      vector<action>  context_free_actions;
      vector<action>  actions;

      EOSLIB_SERIALIZE( transaction, (expiration)(region)(ref_block_num)(ref_block_prefix)(net_usage_words)(kcpu_usage)(delay_sec)(context_free_actions)(actions) )
   };

   class deferred_transaction : public transaction {
      public:
         uint128_t     sender_id;
         account_name  sender;
         account_name  payer;
         time          delay_until;

         static deferred_transaction from_current_action() {
            return unpack_action_data<deferred_transaction>();
         }

         EOSLIB_SERIALIZE_DERIVED( deferred_transaction, transaction, (sender_id)(sender)(payer)(delay_until) )
   };

   /**
    * Retrieve the indicated action from the active transaction.
    * @param type - 0 for context free action, 1 for action
    * @param index - the index of the requested action
    * @return the indicated action
    */
   action get_action( uint32_t type, uint32_t index ) {
      auto size = ::get_action(type, index, nullptr, 0);
      eosio_assert( size > 0, "get_action size failed" );
      char buf[size];
      auto size2 = ::get_action(type, index, &buf[0], static_cast<size_t>(size) );
      eosio_assert( size == size2, "get_action failed" );
      return eosio::unpack<eosio::action>(&buf[0], static_cast<size_t>(size));
   }

   ///@} transactioncpp api

} // namespace eos
