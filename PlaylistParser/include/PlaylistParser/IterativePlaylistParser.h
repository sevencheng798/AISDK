/*
 * Copyright 2019 its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef _PLAYLISTPARSER_ITERATIVEPLAYLISTPARSER_H_
#define _PLAYLISTPARSER_ITERATIVEPLAYLISTPARSER_H_

#include <deque>
#include <memory>

#include <DMInterface/HTTPContentFetcherInterfaceFactoryInterface.h>
#include <Utils/PlaylistParser/IterativePlaylistParserInterface.h>

#include "PlaylistParser/PlaylistUtils.h"

namespace aisdk {
namespace playlistParser {

/**
 * This playlist parser implements an iterative depth first search algorithm to find audio urls inside a playlist url.
 * Each call to next will perform the search until it hits a leaf (a valid media). When the playlist is empty, it will
 * set the parse result to SUCCESS.
 *
 * This class is not thread safe except for method @c abort(). This method can be called while another thread is running
 * and it will cause ongoing and future calls to @c next to fail. Calls to @c initializeParsing will reset abort state.
 */
class IterativePlaylistParser : public utils::playlistParser::IterativePlaylistParserInterface {
public:
    /**
     * Creates a new @c IterativePlaylistParser instance.
     *
     * @param contentFetcherFactory A factory that can create @c HTTPContentFetcherInterfaces.
     * @return An @c std::unique_ptr to a new @c IterativePlaylistParser if successful or @c nullptr otherwise.
     */
    static std::unique_ptr<IterativePlaylistParser> create(
        std::shared_ptr<dmInterface::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory);

    /// @name IterativePlaylistParserInterface methods.
    /// @{
    bool initializeParsing(std::string url) override;
    PlaylistEntry next() override;
    void abort() override;
    /// @}

private:
    /**
     * Constructor.
     *
     * @param contentFetcherFactory The object that will be used to create objects with which to fetch content from
     * urls.
     */
    IterativePlaylistParser(
        std::shared_ptr<dmInterface::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory);

    /**
     * Retrieves playlist content and stores it into a string.
     *
     * @param contentFetcher Object used to retrieve url content.
     * @param [out] content The playlist content.
     * @return @c true if no error occured or @c false otherwise.
     * @note This function should be used to retrieve content specifically from playlist URLs. Attempting to use this
     * on a media URL could be blocking forever as the URL might point to a live stream.
     */
    bool getPlaylistContent(
        std::unique_ptr<dmInterface::HTTPContentFetcherInterface> contentFetcher,
        std::string* content);

    /// Used to retrieve content from URLs
    std::shared_ptr<dmInterface::HTTPContentFetcherInterfaceFactoryInterface> m_contentFetcherFactory;

    /// A queue with the URLs that still require parsing. This object is used to save the traversal state.
    std::deque<UrlAndInfo> m_urlsToParse;

    /// A string to the last url parsed.
    std::string m_lastUrl;

    /// A flag used to abort an ongoing playlist parsing.
    std::atomic_bool m_abort;
};

}  // namespace playlistParser
}  // namespace aisdk

#endif  // _PLAYLISTPARSER_ITERATIVEPLAYLISTPARSER_H_