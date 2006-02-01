/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2001, 2002, 2003 Sadruddin Rejeb

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file calibrationhelper.hpp
    \brief Calibration helper class
*/

#ifndef quantlib_interest_rate_modelling_calibration_helper_h
#define quantlib_interest_rate_modelling_calibration_helper_h

#include <ql/grid.hpp>
#include <ql/pricingengine.hpp>
#include <ql/PricingEngines/blackmodel.hpp>

namespace QuantLib {

    //! liquid market instrument used during calibration
    class CalibrationHelper : public Observer, public Observable {
      public:
        CalibrationHelper(const Handle<Quote>& volatility,
                          const Handle<YieldTermStructure>& termStructure,
                          bool calibrateVolatility = false)
        : volatility_         (volatility),
          termStructure_      (termStructure),
          calibrateVolatility_(calibrateVolatility) {
            blackModel_ = boost::shared_ptr<BlackModel>(
                                  new BlackModel(volatility_,termStructure_));
            registerWith(volatility_);
            registerWith(termStructure_);
        }
        void update() {
            marketValue_ = blackPrice(volatility_->value());
            notifyObservers();
        }

        //! returns the actual price of the instrument (from volatility)
        Real marketValue() const { return marketValue_; }

        //! returns the price of the instrument according to the model
        virtual Real modelValue() const = 0;

        //! returns the error resulting from the model valuation
        virtual Real calibrationError();

        virtual void addTimesTo(std::list<Time>& times) const = 0;

        //! Black volatility implied by the model
        Volatility impliedVolatility(Real targetValue,
                                     Real accuracy,
                                     Size maxEvaluations,
                                     Volatility minVol,
                                     Volatility maxVol) const;

        //! Black price given a volatility
        virtual Real blackPrice(Volatility volatility) const = 0;

        void setPricingEngine(const boost::shared_ptr<PricingEngine>& engine) {
            engine_ = engine;
        }

      protected:
        Real marketValue_;
        Handle<Quote> volatility_;
        Handle<YieldTermStructure> termStructure_;
        boost::shared_ptr<BlackModel> blackModel_;
        boost::shared_ptr<PricingEngine> engine_;

      private:
        class ImpliedVolatilityHelper;
        const bool calibrateVolatility_;
    };

}


#endif
